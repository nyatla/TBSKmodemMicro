#pragma once

//compatibility
#if defined(_WIN_DEBUG)
#include <ctime>
#include <iostream>

    void randomSeed(int v) {};
    int random(int r) {
        return 0x55;
    };
    void digitalWrite(int pin, int v) {
        printf("DW:%d\n", v);
    }
    time_t micros()
    {
        return clock();
    }
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


    template <typename T> class IPyIterator
    {
    public:
        virtual ~IPyIterator() {}
        virtual T next() = 0;
        virtual bool hasNext() = 0;
    };
    class IBitIterator:public IPyIterator<TMM_UINT8> {
    };

}


