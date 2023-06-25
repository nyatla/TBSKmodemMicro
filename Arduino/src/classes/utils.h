#pragma once

#if defined(_WIN_DEBUG)
    #include "windebug.h"
#else
    #include <Arduino.h>
#endif


#if defined(ASSERT)
    #define TBSK_ASSERT(E) ASSERT(E)
#else
    #define TBSK_ASSERT(E)
#endif

namespace TBSKmodemMicro
{
    typedef signed char     TMM_INT8;
    typedef signed short    TMM_INT16;
    typedef signed int      TMM_INT32;
    typedef unsigned char   TMM_UINT8;
    typedef unsigned short  TMM_UINT16;
    typedef unsigned int    TMM_UINT32;


    /** 累積待機時間がタイマ精度の1/8を目指す待機関数
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
         * 全開の呼び出しから4096us以上が経過した場合は正常に動作しない。
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




    template <typename T> class IPyIterator
    {
    public:
        virtual ~IPyIterator() {}
        virtual T next() = 0;
        virtual bool hasNext() = 0;
    };
    class IBitIterator:public IPyIterator<TMM_UINT8> {
    };


    class BitArray :public IPyIterator<TMM_UINT8> {
    private:
        TMM_UINT16 _c;
    public:
        const TMM_UINT16 size;
    protected:
        inline virtual const TMM_UINT8* refBuf()const = 0;
    public:
        BitArray(TMM_UINT16 size) :size{ size }, _c{ 0 }
        {}
        TMM_UINT8 get(int i)const
        {
            TBSK_ASSERT(i < this->size);
            return (this->refBuf()[i / 8] >> (7 - i % 8)) & 0x01;
        }
        bool hasNext(){
            auto c = this->_c;
            if (c >= this->size) {
                return false;
            }
            return true;
        }
        TMM_UINT8 next()
        {
            auto c = this->_c;
            TBSK_ASSERT(c < this->size);
            TMM_UINT8 r = this->refBuf()[c / 8] >> (7 - c % 8);
            this->_c = c + 1;
            return r & 0x01;
        }
        /** 参照位置をリセットする。
        */
        void reset() {
            this->_c = 0;
        }

    };
    template <size_t SIZE> class RawBits :public BitArray {
    protected:
        TMM_UINT8 b[SIZE];//ココサイズおかしくない
    protected:
        inline const TMM_UINT8* refBuf()const { return this->b; };
    public:
        RawBits() :BitArray(SIZE){
            memset(b, 0, SIZE * sizeof(TMM_UINT8));
        }
        void set(int i, int v) {
            auto n = (v & 0x01) << (7 - i % 8);
            this->b[i / 8] = (n == 0) ? (this->b[i / 8] & (~n)) : (this->b[i / 8] | n);
        }
    };



}


