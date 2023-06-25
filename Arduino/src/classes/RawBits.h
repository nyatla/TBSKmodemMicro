#pragma once
#include "types.h"
#include<memory>

namespace TBSKmodemMicro
{

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


