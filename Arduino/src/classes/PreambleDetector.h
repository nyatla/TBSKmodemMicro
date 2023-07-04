#pragma once
#include "./RingBuffer16.h"
#include "./FF16SelfCorrcoefIterator.h"

#include <memory>



#define FLOAT_TO_FF32(v) ((TMM_INT32)(v>0?v*32767:v*32768))	//FLOATを16bit固定小数点 s.0.15 [-1,1]に変換する
#define FLOAT_TO_FF16(v) ((TMM_INT16)(v>0?v*32767:v*32768))	//FLOATを16bit固定小数点 s.0s15 [-32768,32767]に変換する
#define FF16_TO_FLOAT(v) ((TMM_INT16)((v>0?(float)v/32767:(float)v/32768)))	//FLOATを16bit固定小数点 s.0s15 [-32768,32767]に変換する

namespace TBSKmodemMicro
{
	/// <summary>
	/// 最大で0xffff個のs.0.15bit整数の平均値を記録するRingBuffer
	/// </summary>
	/// <typeparam name="NOFAVE"></typeparam>
	/// <typeparam name="NOFBUF"></typeparam>
	template <TMM_UINT16 NOFAVE, TMM_UINT16 NOFBUF> class AveLog :public RingBuffer16<TMM_INT16, NOFBUF>
	{
	private:
		TMM_INT32 _sum;
		RingBuffer16<TMM_INT16, NOFAVE> _rb;
	public:
		AveLog() :RingBuffer16<TMM_INT16, NOFBUF>(0), _rb(0), _sum(0)
		{
		}
		void append(TMM_INT16 v)
		{
			TMM_INT16 a = this->_rb.getTop();
			this->_rb.append(v);
			auto old = this->_sum;
			this->_sum = this->_sum + (TMM_INT32)v - (TMM_INT32)a;
			RingBuffer16<TMM_INT16, NOFBUF>::append((TMM_INT16)(this->_sum / this->_rb.getLength()));
		}
	};

	/// <summary>
	/// 最大で0xffff個のINT16を記録するリングバッファ
	/// 入力値は FF16 s.0.15 [-1,1]
	/// </summary>
	/// <typeparam name="NOFBUF"></typeparam>
	template <TMM_UINT16 NOFBUF> class TickLog :public RingBuffer16<TMM_INT16, NOFBUF> {
	private:

	public:
		TickLog() :RingBuffer16<TMM_INT16, NOFBUF>(0) {
		}
		void indexOfSum3Max(TMM_UINT16 size_back, TMM_UINT32* out_max_i, TMM_INT32* out_max_v)
		{
			//assert(size_back > 0)
			TMM_UINT16 buflen = this->getLength();
			//# 探索開始位置 RBの後端からsize_back戻ったところ
			RefIterator<TMM_INT16> v_siter;
			auto& siter = this->subIter(buflen - size_back, size_back, v_siter);
			TMM_INT16 a[3] = {};
			for (auto i = 0;i < 3;i++) {
				a[i] = siter.next();

			}
			TMM_UINT32 max_i = 0;
			TMM_INT32 max_v = (TMM_INT32)a[0] + (TMM_INT32)a[1] + (TMM_INT32)a[2];
			TMM_UINT32 n = 0;
			while (siter.hasNext()) {
				TMM_INT32 s = (TMM_INT32)a[0] + (TMM_INT32)a[1] + (TMM_INT32)a[2];
				auto _sum = siter.next();
				if (_sum > max_v) {
					max_i = n + 1;
					max_v = _sum;
				}
				a[n % 3] = _sum;
				//printf("%d,%f\n", n, FF16_TO_FLOAT(s));
				n = n + 1;
			}
			*out_max_i = max_i + 1;
			*out_max_v = max_v;
			return;
		}
		//""" 過去N個の中で最大の値とインデクスを探す.
		//    探索範囲は, +1, n - 1となる。
		//    戻り値は[-(size_back - 1), 0]
		//"""
		TMM_INT16 max(TMM_UINT16 start, TMM_UINT16 size)
		{
			//assert(size > 0)
			RefIterator<TMM_INT16> v_siter;
			auto& siter = this->subIter(start, size, v_siter);
			TMM_INT16 max_v = siter.next();
			while (siter.hasNext()) {
				auto v = siter.next();
				if (v > max_v) {
					max_v = v;
				}
			}
			return max_v;
		}
		TMM_INT16 min(TMM_UINT16 start, TMM_UINT16 size)
		{
			//assert(size > 0)
			RefIterator<TMM_INT16> v_siter;
			auto& siter = this->subIter(start, size, v_siter);
			TMM_INT16 min_v = siter.next();
			while (siter.hasNext()) {
				auto v = siter.next();
				if (v < min_v) {
					min_v = v;
				}
			}
			return min_v;
		}
		TMM_INT16 ave(TMM_UINT16 start, TMM_UINT16 size) {
			//assert(size > 0)
			TMM_INT32 s = 0;
			RefIterator<TMM_INT16> v_siter;
			auto& siter = this->subIter(start, size, v_siter);
			while (siter.hasNext()) {
				s = s + siter.next();
			}
			return (TMM_INT16)(s / size);
		}
	};





	template <TMM_UINT16 CYCLE, TMM_UINT16 SYMBOL_TICKS> class CoffPreambleDetector
	{
	private:
		AveLog<SYMBOL_TICKS, SYMBOL_TICKS* (CYCLE + 1)> _average1;	// FF16	s.0.15
		TickLog<SYMBOL_TICKS* (6 + CYCLE * 2)> _tickbuf;			// FF16	s.0.15
		FF16SelfCorrcoefIterator<SYMBOL_TICKS, SYMBOL_TICKS> _cof;
		TMM_INT32 _next_value;
		bool _has_value;
		bool _has_next;
		TMM_INT16 _threshold;
		/// <summary>
		/// srcはFF16 s.0.15
		/// </summary>
		/// <param name="src"></param>
		/// <param name="threshold"></param>
	public:
		CoffPreambleDetector(IPyIterator<TMM_INT16>& src, float threshold):
			_average1(AveLog<SYMBOL_TICKS, SYMBOL_TICKS* (CYCLE + 1)>()),
			_tickbuf(TickLog<SYMBOL_TICKS* (6 + CYCLE * 2)>()),
			_cof(FF16SelfCorrcoefIterator<SYMBOL_TICKS, SYMBOL_TICKS>()),
			_next_value(false),
			_has_value(false),
			_has_next(0),
			_threshold(FF16_TO_FLOAT(threshold))
		{
			this->_cof.init(src);
		}
		TMM_INT32 next() {
			TBSK_ASSERT(this->_has_value);
			this->_has_value = false;
			return this->_next_value;
		}
		bool hasNext()
		{
			if (this->_has_value) {
				return true;
			}
			auto& cof = this->_cof;
			auto& ave1 = this->_average1;
			auto& tickbuf = this->_tickbuf;
			for (;;)
			{
				TMM_INT32 nor = 0;
				TMM_INT32 gap = 0;//-2,2を取る
				for (;;) {
					if (!cof.hasNext()) {
						//BREAK
						this->_has_value = false;
						return false;
					}
					auto a = cof.next();
					ave1.append(a);
					tickbuf.append(a);
					nor = nor + 1;
					gap = ((TMM_INT32)ave1.getTop()) - ((TMM_INT32)ave1.getTail());
					if (gap < FLOAT_TO_FF32(0.5)) {
						continue;
					}
					if (ave1.getTop() < FLOAT_TO_FF16(0.1)) {
						continue;
					}
					if (ave1.getTail() > FLOAT_TO_FF16(-0.1)) {
						continue;
					}
					break;
				}
				//print(ave1.tail, ave1.top) #  - 0.25432290820230913 0.27101677789788603
				//# ギャップ最大化
				for (;;) {
					if (!cof.hasNext()) {
						//BREAK
						this->_has_value = false;
						return false;
					}
					TMM_INT16 a = cof.next();

					ave1.append(a);
					tickbuf.append(a);

					nor = nor + 1;
					auto w1 = ave1.getTop();
					auto w2 = ave1.getTail();
					TMM_INT32 w = ((TMM_INT32)ave1.getTop()) - ((TMM_INT32)ave1.getTail());
					if (w >= gap) {
						gap = w;
						continue;
					}
					break;
				}
				if (gap < this->_threshold) {
					continue;//コルーチンをリセット
				}
				//# print(2, nor, self._gap)
				TMM_INT16 pmax = ave1.getTail();
				//print(ave1.tail, ave1.top) #   - 1.0 0.9995798842588445

				for (;;) {
					if (!cof.hasNext()) {
						//BREAK
						this->_has_value = false;
						return false;
					}
					TMM_INT16 a = cof.next();
					ave1.append(a);
					tickbuf.append(a);
					TMM_INT16 n = ave1.getTail();
					nor = nor + 1;
					if (n > pmax) {
						pmax = n;
						continue;
					}
					if (pmax > 0.1) {
						break;
					}
				}
				TMM_UINT16 sample_width = CYCLE + 1;


				TMM_UINT16 cofbuf_len = tickbuf.getLength();
				//				TMM_INT16 iom[2] = {};
				TMM_UINT32 out_max_i;
				TMM_INT32 out_max_v;
				tickbuf.indexOfSum3Max(SYMBOL_TICKS, &out_max_i, &out_max_v);


				//#ピークを基準に詳しく様子を見る。
				TMM_INT32 peak_pos2 = (TMM_INT32)out_max_i + nor - SYMBOL_TICKS - 1;
				//print("peak_pos2", peak_pos2)# 2:1299
				TMM_INT32 s;
				s = peak_pos2 - SYMBOL_TICKS * sample_width - (nor - cofbuf_len);
				if ((TMM_INT32)tickbuf.ave((TMM_INT16)s, CYCLE * SYMBOL_TICKS) > ((TMM_INT32)tickbuf.min(s, CYCLE * SYMBOL_TICKS) * FLOAT_TO_FF32(0.66)) >> 15) {
					continue; //#バラツキ大きい
				}
				s = peak_pos2 - SYMBOL_TICKS * sample_width * 2 - (nor - cofbuf_len);
				if ((TMM_INT32)tickbuf.ave((TMM_INT16)s, CYCLE * SYMBOL_TICKS) < ((TMM_INT32)tickbuf.max(s, CYCLE * SYMBOL_TICKS) * FLOAT_TO_FF32(0.66)) >> 15) {
					continue; //#バラツキ大きい
				}
				//#値の高いのを抽出してピークとする。
				//print(peak_pos2 - self._nor)#  - 54, 2 = -55
				this->_has_value = true;
				this->_next_value = peak_pos2 - nor;//#現在値からの相対位置
				return true;
			}

		}
	};
}
