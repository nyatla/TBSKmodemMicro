#pragma once
#include "types.h"
#include "RawBits.h"

namespace TBSKmodemMicro
{



    template <size_t SIZE> class PnBits :public RawBits<SIZE> {
    public:
        PnBits() :RawBits<SIZE>() {}
        void setRandom(TMM_UINT16 seed) {
            randomSeed(seed);
            for (auto i = 0;i < (SIZE + 7) / 8;i++) {
                this->b[i] = random(255);
            }
            return;
        }
    };

}


