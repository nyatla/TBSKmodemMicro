#pragma once
#include "./types.h"


namespace TBSKmodemMicro
{

    /** 累積待機時間から、タイマ精度の1/8の待機時間を目指す待機関数
     *
     */
    class TickTack {
        TMM_UINT16 _interval;// = 1 / 8; // 1/8 us tick
        TMM_UINT16 _expire = 0;
    public:
        /**
         * この関数を呼び出してからwaitを呼び出すまでの時間は4096us以内にすること。
         * interval_in_3dusは1/8us単位の待機時間
         */
        TickTack(TMM_UINT16 interval_in_3dus) {
            TBSK_ASSERT(interval_in_3dus < ((4096 << 3) - 1));
            this->_interval = interval_in_3dus;
            this->reset();
        }
        void reset() {
            this->_expire = ((0x1FFF & micros()) << 3) + this->_interval;//init
        }
        /**
         * 最後にwait,またはresetを呼び出してからinterval以上経過するまでまつ。
         * 前回の呼び出しから4096us以上が経過した場合は正常に動作しない。
         */
        void wait() {
            TMM_UINT16 next = this->_expire + this->_interval;
            TMM_UINT16 now = (0x1FFF & micros()) << 3;//init
            TMM_UINT16 expire = this->_expire;
            TMM_UINT16 d = expire - now;
            if (d >= 0x7FFFF) {
                //expired
                this->_expire = next;
                return;
            }
            while (true) {
                TMM_UINT16 now = (0x1FFF & micros()) << 3;//init
                if ((TMM_UINT16)(expire - now) > d) {
                    this->_expire = next;
                    return;
                }
            }
        }
    };
}


