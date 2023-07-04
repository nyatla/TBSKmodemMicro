#pragma once
#include "./PreambleDetector.h"
#include "./AverageInterator16.h"
#include "./TraitBlockDecoder.h"
#include "./TickTack.h"



using namespace TBSKmodemMicro;



namespace TBSKmodemMicro
{



	/// <summary>
	/// TMM_INT16配列を返す
	/// </summary>
	class BufIterator :public IPyIterator<TMM_INT16>
	{
	private:
		const TMM_INT16* _buf;
		const TMM_UINT16 _size;
		TMM_UINT16 _pos;
	public:
		BufIterator(const TMM_INT16* buf, TMM_UINT16 size) :_buf(buf), _size(size), _pos(0)
		{
		}
		TMM_INT16 next() {
			TBSK_ASSERT(this->hasNext());
			auto p = this->_pos;
			this->_pos++;
			return this->_buf[p];
		};
		bool hasNext() {
			return this->_pos < this->_size;
		};
	};




	/// <summary>
	/// 最大返却数に制限のあるイテレータ
	/// </summary>
	class LimitedIter :public IPyIterator<TMM_INT16>
	{
	private:
		IPyIterator<TMM_INT16>& _src;
		TMM_INT32 _limit;
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="src"></param>
		/// <param name="limit">返却値の最大数。-1でinfinite</param>
		LimitedIter(IPyIterator<TMM_INT16>& src, TMM_INT32 limit) :_src(src), _limit(limit)
		{
		}
		TMM_INT16 next() {
			if (this->_limit > 0) {
				this->_limit--;
				return this->_src.next();
			}
			else if (this->_limit == -1) {
				return this->_src.next();
			}
			else {
				return 0; //OMG
			}
		}
		bool hasNext() {
			if (this->_limit > 0 || this->_limit == -1) {
				return this->_src.hasNext();
			}
			return false;
		}
		/// <summary>
		/// 制限を解除する
		/// </summary>
		void clear() {
			this->_limit = 0xffffffff;
		}
	};


	class DigitalInIterator :public IPyIterator<TMM_INT16>
	{
	private:
		TickTack _ticktack;
		int _pin;
		int _adc_bits;
	public:
		DigitalInIterator(int pin, TMM_UINT16 carrier,int adc_bits=12) :
			_ticktack(TickTack((1000 << 3) / (carrier / 1000))),
			_pin(pin),
			_adc_bits(adc_bits)
		{
			this->_ticktack.reset();
		}
		TMM_INT16 next()
		{
			this->_ticktack.wait();
			TMM_INT32 of = (TMM_INT32)1 << (this->_adc_bits - 1);
			TMM_INT32 v = (((TMM_INT32)analogRead(this->_pin)) - of);
			return v>0x7ffff?0x7ffff:(v<-0x7ffff?-0x7ffff:v);
		};
		bool hasNext()
		{
			return true;
		};
		void clear() {

		}
	};







	/// <summary>
	/// TBSK信号を同期して復調します。
	/// </summary>
	/// <param name="src"></param>
	/// <param name="buf"></param>
	/// <param name="buf_len"></param>
	/// <returns></returns>
	template <TMM_INT16 TONE_SIZE> class TbskDemodulator
	{
	public:
		/// <summary>
		/// コールバックインタフェイス
		/// コールバック関数は実行状態を通知しますが,同期的です。
		/// 搬送波数周波数の間隔よりも短い時間で終了しなければなりません。
		/// </summary>
		class ReadCallback {
		public:
			virtual void onSignal()const = 0;
			virtual void onData()const = 0;
			virtual void onLost()const = 0;
		};
	public:
		TbskDemodulator() {
		}
		/// <summary>
		/// srcの振幅データをデコードしてbufに格納して返します。
		/// デコードしたデータの最大長はbuf_lenです。
		/// </summary>
		/// <param name="src"></param>
		/// <param name="src_len"></param>
		/// <param name="buf"></param>
		/// <param name="buf_len"></param>
		/// <returns></returns>
		int demodulate(IPyIterator<TMM_INT16>& src, void* buf, int buf_len)
		{
			//トリガ検出
			TMM_INT32 pd_pos = 0;
			{
				CoffPreambleDetector<4, TONE_SIZE> pd(src, 1.0f);
				if (!pd.hasNext()) {
					return -1;
				}
				pd_pos = pd.next();
			}
			//スキップ
			{
				for (size_t i = 0;i < TONE_SIZE + pd_pos;i++) {
					if (!src.hasNext()) {
						return 0;
					}
					src.next();
				}
				//デコード
				TraitBlockDecoder<TONE_SIZE> bd(src, 0.2f);
				for (int i = 0;i < buf_len;i++) {
					TMM_UINT8 w = 0;
					for (auto j = 0;j < 8;j++) {
						if (!bd.hasNext()) {
							return i;
						}
						w = (w << 1) | bd.next();
					}
					((TMM_UINT8*)buf)[i] = w;
				}
				return buf_len;
			}
		}
		int demodulate(const short src[], int src_len, void* buf, TMM_UINT32 buf_len)
		{
			BufIterator src_((TMM_INT16*)src, src_len);
			return this->demodulate(src_, buf, buf_len);
		}
		int read(int pin, int carrier,void* buf, int buf_len, int timeout_in_ms) {
			return this->read(pin, carrier,buf, buf_len, timeout_in_ms,NULL);
		}
		/// <summary>
		/// この関数は、最大でtimeout_in_msだけ信号の受信を待機して、
		/// 検出された場合に、最大でbuf_lenバイトのデータを読み取ります。
		/// callbackが非NULLの場合は同期してイベントを通知します。
		/// </summary>
		int read(int pin, int carrier, void* buf, int buf_len, int timeout_in_ms,const ReadCallback* callback)
		{
			DigitalInIterator di(pin,carrier);
			{
				TMM_INT32 pd_pos = 0;
				//制限付きIterを構成
				LimitedIter src(di, (TMM_INT32)(carrier * timeout_in_ms / 1000));
				{	//制限付きで検出
					CoffPreambleDetector<4, TONE_SIZE> pd(src, 1.0f);
					if (!pd.hasNext()) {
						return -1;
					}
					pd_pos = pd.next();
				}
				//信号の先頭まで移動
				for (size_t i = 0;i < TONE_SIZE + pd_pos;i++) {
					if (!src.hasNext()) {
						return false;
					}
					src.next();
				}
			}
			if (callback) {
				callback->onSignal();
			}
			TraitBlockDecoder<TONE_SIZE> bd(di, 0.2f);
			TMM_UINT8* b = (TMM_UINT8*)buf;
			int i = 0;
			for (;i < buf_len && bd.hasNext();i++) {
				TMM_UINT8 w = 0;
				for (auto j = 0;j < 8;j++) {
					if (!bd.hasNext()) {
						return i;
					}
					w = (w << 1) | bd.next();
				}
				*(b + i) = w;
			}
			if (callback) {
				callback->onLost();
			}
			return i;
		}
	};
}
