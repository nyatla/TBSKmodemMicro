#pragma once
#include "types.h"
#include "../../src/classes/RingBuffer16.h"

#include <memory>
namespace TBSKmodemMicro
{
	/// <summary>
	/// 整数専用の平方根値計算関数
	/// インスタンスのスタックの消費量は((WINDOW*2+SHIFT)+3)*2+5*4
	/// </summary>
	/// <param name="num"></param>
	/// <returns></returns>
	TMM_INT32 sqrtint(TMM_INT32 num)
	{
		if (num == 0 || num == 1) {
			return num;
		}
		int left = 1;
		int right = num;
		TMM_INT32 result = 0;

		while (left <= right) {
			TMM_INT32 mid = left + (right - left) / 2;

			// 平方根が整数となる場合
			if (mid == num / mid) {
				return mid;
			}

			// 平方根が整数より小さい場合
			if (mid <= num / mid) {
				left = mid + 1;
				result = mid;
			}
			else {
				right = mid - 1;
			}
		}

		return result;
	}
	/// <summary>
	/// 相関値を計算する。出力精度は16bitになる。入力はFF16 s.0.15
	/// 100Ticks以上は処理できないと思う。
	/// </summary>
	/// <typeparam name="SHIFT"></typeparam>
	/// <typeparam name="WINDOW"></typeparam>
	template <TMM_UINT16 WINDOW, TMM_UINT16 SHIFT> class FF16SelfCorrcoefIterator :public IPyIterator<TMM_INT16>
	{
	private:
		TMM_INT16 xyi_x[WINDOW];
		TMM_INT16 xyi_y[WINDOW];
		TMM_INT16 c;
		TMM_INT16 n;
		TMM_INT32 sumxi;
		TMM_INT32 sumxi2;
		TMM_INT32 sumyi;
		TMM_INT32 sumyi2;
		TMM_INT32 sumxiyi;
		IPyIterator<TMM_INT16>* _srcx;
		TMM_INT16 _srcy[SHIFT];
		TMM_INT16 _srcy_ptr;
	public:
		FF16SelfCorrcoefIterator() : c{ 0 }, n{ 0 }, sumxi{ 0 }, sumxi2{ 0 }, sumyi{ 0 }, sumyi2{ 0 }, sumxiyi{ 0 }, _srcx{ NULL }, _srcy_ptr{ 0 }
		{
			memset(this->xyi_x, 0, sizeof(this->xyi_x));
			memset(this->xyi_y, 0, sizeof(this->xyi_y));
			memset(this->_srcy, 0, sizeof(this->_srcy));
		}
		void init(IPyIterator<TMM_INT16>& src) {
			TBSK_ASSERT(this->_srcx == NULL);
			this->_srcx = &src;
		}
		/// <summary>
		/// 確保済みインスタンスの初期化
		/// </summary>
		/// <returns></returns>
		bool hasNext() override {
			return this->_srcx->hasNext();

		}
		TMM_INT16 next()override
		{
			const TMM_INT16 FP15 = 15;	//15 bit signed ff
			const TMM_INT16 FP7 = 7;	//7 bit signed ff

			const auto l = WINDOW;
			TMM_INT16 m = this->c;
			this->c = (this->c + 1) % l;
			TMM_INT16 vx = this->_srcx->next();
			TMM_INT16 srcy_ptr = this->_srcy_ptr;
			TMM_INT16 vy = this->_srcy[srcy_ptr];
			this->_srcy[srcy_ptr] = vx;
			this->_srcy_ptr = (this->_srcy_ptr + 1) % SHIFT;


			TMM_INT32 lxi = this->xyi_x[m];
			TMM_INT32 lyi = this->xyi_y[m];
			if (this->n < l) {
				this->n += 1;
			}
			TMM_INT16 n = this->n;
			this->sumxi += ((TMM_INT32)vx) - lxi;	//s+8+15bit
			this->sumxi2 += (((TMM_INT32)vx * (TMM_INT32)vx) >> FP15) - ((lxi * lxi) >> FP15);	//s+8+15bit
			this->sumyi += ((TMM_INT32)vy) - lyi;	//s+8+15bit
			this->sumyi2 += (((TMM_INT32)vy * (TMM_INT32)vy) >> FP15) - ((lyi * lyi) >> FP15);	//s+8+15bit
			this->sumxiyi += (((TMM_INT32)vx * (TMM_INT32)vy) >> FP15) - ((lxi * lyi) >> FP15);	//s+8+15bit

			this->xyi_x[m] = vx;	//s+15bit
			this->xyi_y[m] = vy;	//s+15bit


			if (n == 1) {
				return 1;
			}
			
			TMM_INT32 sumxi = this->sumxi>>8;	//s+8+8bit精度
			TMM_INT32 v1 = (this->sumxi2>>8) - ((sumxi * sumxi / n) >> FP7);
			if (v1 <= 0) {
				return 0;
			}
			TMM_INT32 stdx = sqrtint((v1<< FP7) /(n - 1));
			if (stdx <= 0) {
				return 0;
			}

			TMM_INT32 sumyi = this->sumyi>>8; //s+8bit精度
			TMM_INT32 v2 = (this->sumyi2>>8) - ((sumyi * sumyi / n) >> FP7);
			if (v2 <= 0) {
				return 0;
			}
			TMM_INT32 stdy = sqrtint((v2 << FP7) / (n - 1));
			if (stdy <= 0) {
				return 0;
			}

			TMM_INT32 sumxiyi = this->sumxiyi >> 8;

			TMM_INT32 a = ((sumxiyi << FP7) + (-sumxi * sumyi) / (n * n)) << FP7;
			TMM_INT32 b = (n - 1) * stdx * stdy;
			TMM_INT32 r = a/b;
			return (TMM_INT16)((r > 127 ? 127 : (r < -128 ? -128 : r)))<<8;
		}
	};
}


