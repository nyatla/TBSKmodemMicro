#pragma once
#include "./types.h"
#include "./SumIterator16.h"
namespace TBSKmodemMicro
{

    template <TMM_UINT16 LENGTH> class AverageInterator16 :public SumIterator16<LENGTH>
    {
    private:
        //このイテレータはRecoverableStopInterationを利用できます。
    public:
        AverageInterator16(IPyIterator<TMM_INT16>& src) :SumIterator16<LENGTH>(src)
        {}
        TMM_INT32 next()override {
            return SumIterator16<LENGTH>::next() / this->_buf.getLength();
        };
        bool hasNext()override {
            return SumIterator16<LENGTH>::hasNext();
        };

    };

    
}
