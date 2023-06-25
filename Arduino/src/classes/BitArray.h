#pragma once
#include "types.h"



namespace TBSKmodemMicro
{


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

	



}


