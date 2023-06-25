#pragma once
#include "./types.h"
#include "./PreambleIterator.h"
#include "./BitArray.h"
#include "./DataBits.h"
#include "./PnBits.h"
#include "./TickTack.h"

namespace TBSKmodemMicro
{

	

    /**
     */
    class PulseIter :public IBitIterator
    {
    private:
        TMM_UINT16 _n;
        TMM_UINT16 _co;
        TMM_UINT16 _tval;
        PreambleIter _preamble;
        BitArray* _tone;
        DataBits _data;

    public:
        PulseIter(TMM_UINT16 preamble_cycle) :
            _tval{ 0 }, _co{ 0 }, _n{ 0 }, _tone{nullptr},_preamble(preamble_cycle)
        {
        }
        void init(BitArray* ref_tone,const TMM_UINT8* data, size_t size) {
            this->_tone = ref_tone;
            this->_preamble.init();
            this->_data.init(data, size);
            this->_co = 0;
            this->_n = 0;
        }
        bool hasNext()override {
            return this->_co < 4;
        }
        TMM_UINT8 next()
        {
            const auto TONE_SIZE=this->_tone->size;
            for (;;) {
                switch (this->_co) {
                case 0: //Prefix:1bit
                    if (this->_n >= TONE_SIZE) {
                        this->_co++;
                        this->_tval = this->_preamble.next();//reset
                        this->_tone->reset();
                        continue;
                    }
                    this->_n++;
                    return (this->_n % 3) & 0x01;
                case 1: //Sync:14bit (2+4+1+4+3)
                    if (!this->_tone->hasNext()) {
                        if (!this->_preamble.hasNext()) {
                            this->_co++;
                            continue;
                        }
                        this->_tone->reset();
                        this->_tval = this->_preamble.next();
                    }
                    return this->_tval ^ this->_tone->next();
                case 2: //Data: (1+n)bit
                    if (!this->_tone->hasNext()) {
                        if (!this->_data.hasNext()) {
                            this->_co++;
                            this->_n = 0;
                            continue;
                        }
                        this->_tone->reset();
                        this->_tval=this->_data.next();
                    }
                    return this->_tval ^ this->_tone->next();
                case 3: //Padding 1bit
                    if (this->_n >= TONE_SIZE-1) {//1つまえでcoを切り替える
                        this->_co++;
                    }
                    this->_n++;
                    return (this->_n % 3) & 0x01;
                default:
                    TBSK_ASSERT(false);
                }
            }

        }
    };
    
    template <int TONE_SIZE=100>  class TbskPulseModulator
    {
        PnBits<TONE_SIZE> _pn;
        PulseIter _pulse;
    public:
        TbskPulseModulator(int preamble_cycle=4,int seed = 299) :_pulse{preamble_cycle} {
            this->_pn.setRandom((TMM_INT16)seed);
        }
        IBitIterator& modulate(const char src[],int srclen=-1) {
            this->_pulse.init(&this->_pn, (const TMM_UINT8*)src, (srclen<0?strlen(src):srclen)*8);
            return this->_pulse;
        }
        void write(int pin, int carrier,const char src[], int srclen= -1)
        {
            TMM_UINT16 us3d=(1000 << 3) / (carrier / 1000);
            TickTack tt(us3d);
            auto& iter = this->modulate(src, srclen);
            tt.reset();
            while (iter.hasNext()) {
                tt.wait();
                digitalWrite(pin, iter.next());
            }
        }
    };
}


