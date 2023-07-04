#pragma once










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


//compatibility
#if defined(_WIN_DEBUG)
#include <ctime>
#include <iostream>

void randomSeed(int v) {};
int random(int r) {
    return 0x55;
};
static size_t _buf_len = 0;
static const TBSKmodemMicro::TMM_INT16* _buf = NULL;
static size_t _buf_pos = 0;

void digitalWrite(int pin, int v) {
    printf("DW:%d\n", v);
}
int analogRead(int pin) {
    _buf_pos = (_buf_pos + 1) % _buf_len;
    return _buf[_buf_pos];
}

void dbg_initAnalogData(const TBSKmodemMicro::TMM_INT16* buf, size_t buf_len) {
    _buf_len = buf_len;
    _buf = buf;
    _buf_pos = 0;
}

static unsigned long _micros_counter = 0;
unsigned long micros()
{
    _micros_counter += 10;
    return _micros_counter;
}
#else
#include <Arduino.h>
#endif
