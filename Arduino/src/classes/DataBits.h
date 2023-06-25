#pragma once
#include "types.h"


namespace TBSKmodemMicro
{

	
    /// <summary>
    /// 外部定義したイミュータブルメモリをビットストリームとして取り扱うイテレータです。
    /// </summary>
    class DataBits :public IPyIterator<TMM_UINT8>
    {
    private:
        TMM_UINT16 _c;
        const TMM_UINT8* _data;
        TMM_UINT8 _lastbit;
    private:
        TMM_UINT16 size;
    public:
        DataBits() :_c{ 0 }, _data{ nullptr }, _lastbit{ 0 }, size{0} {}

        void init(const TMM_UINT8* data, size_t size) {
            this->_data = data;
            this->size = (TMM_UINT16)(size);
            this->_lastbit = 0;
            this->_c = 0;
        }
        bool hasNext()override {
            auto c = this->_c;
            if (c > this->size) {
                return false;
            }
            return true;
        }
        TMM_UINT8 next()
        {
            auto c = this->_c;
            this->_c = this->_c + 1;
            if (c == 0) {
                this->_lastbit = 0;
                return 0;
            }
            c = c - 1;
            if (((this->_data[c / 8] >> (7 - c % 8)) & 0x01) == 0) {
                this->_lastbit = (this->_lastbit + 1) % 2;
            }
            return this->_lastbit;
        }
    };


}


