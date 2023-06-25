#pragma once
#include "./types.h"

#include <memory>
namespace TBSKmodemMicro
{


    /// <summary>
    /// 最大要素数に16bit制限のある合計計算機
    /// </summary>
    /// <typeparam name="LENGTH"></typeparam>
    template <TMM_UINT16 LENGTH> class SumIterator16:public IPyIterator<TMM_INT32>
    {
    protected:
        IPyIterator<TMM_INT16>& _src;
        TMM_INT32 _sum;
        RingBuffer16<TMM_INT16, LENGTH> _buf;

    public:
        SumIterator16(IPyIterator<TMM_INT16>& src) :_src{ src }, _sum{ 0 }, _buf{0} {
        }

        TMM_INT32 next()override {
            auto s = this->_src.next();
            auto d = this->_buf.append(s);
            this->_sum = this->_sum + s - d;
            return this->_sum;
        };
        bool hasNext() {
            return this->_src.hasNext();
        };
        const RingBuffer16<TMM_INT16, LENGTH>& getBuf() const {
            return this->_buf;
        }
    };



    
}
