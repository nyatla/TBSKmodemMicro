#pragma once
#include "./utils.h"



namespace TBSKmodemMicro
{

    /// <summary>
    /// Preambleビット列のイテレータです。
    /// </summary>
    class PreambleIter :public IBitIterator {
    private:
        const TMM_UINT16 _cycle;
        TMM_UINT16 _c;
        TMM_UINT16 _n;
        TMM_UINT16 _co;
    public:
        const TMM_UINT16 size;
    public:
        PreambleIter(TMM_UINT16 cycle) :
            _cycle{cycle},
            size{ (TMM_UINT16)(2 + cycle * 2 + 1 + 3) },
            _c{ 0 },
            _n{ 0 },
            _co{ 0 }
        {
        }
        TMM_UINT16 getSize()const { return this->size; }
        void init() {
            this->_n = 0;
            this->_c = 0;
            this->_co = 0;
        }
        bool hasNext() {
            auto c = this->_c;
            if (c >= this->size) {
                return false;
            }
            return true;
        }
        TMM_UINT8 next() {
            auto c = this->_c;
            this->_c++;
            this->_n++;//Resetable counter
            auto n = this->_n;
            switch (this->_co) {
            case 0:
                if (n >= 2) {
                    this->_co++;
                    this->_n = 0;
                }
                return c % 2;//0,1
            case 1:
                if (n >= this->_cycle) {
                    this->_co++;
                    this->_n = 0;
                }
                return 1;//[1]*preamble
            case 2:
                this->_co++;
                this->_n = 0;
                return 1;//[1]
            case 3:
                if (n >= this->_cycle) {
                    this->_co++;
                    this->_n = 0;
                }
                return (n + 1) % 2;//[1|0]
            case 4:
                if (n >= 3) {
                    this->_co++;
                    this->_n = 0;
                }
                return ((n / 3) + (this->_cycle % 2)) % 2;//c,c,!c
            default:
                return 0;//OMG
            }
        }
    };

}


