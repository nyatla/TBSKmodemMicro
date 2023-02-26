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
    typedef unsigned char   TMM_UINT8;
    typedef unsigned short  TMM_UINT16;


    /** 累積待機時間がタイマ精度の1/8を目指す待機関数
     *
     */
    class TickTack {
        TMM_UINT16 _interval;// = 1 / 8; // 1/8 us tick
        TMM_UINT16 _expire = 0;
    public:
        /**
         * この関数を呼び出してからwaitを呼び出すまでの時間は4096us以内にすること。
         * interval_in_3dusは1/8us単位の待機時間
         */
        TickTack(TMM_UINT16 interval_in_3dus) {
            TBSK_ASSERT(interval_in_3dus < ((4096 << 3) - 1));
            this->_interval = interval_in_3dus;
            this->reset();
        }
        void reset() {
            this->_expire = ((0x1FFF & micros()) << 3) + this->_interval;//init
        }
        /**
         * 最後にwait,またはresetを呼び出してからinterval以上経過するまでまつ。
         * 全開の呼び出しから4096us以上が経過した場合は正常に動作しない。
         */
        void wait() {
            TMM_UINT16 next = this->_expire + this->_interval;
            TMM_UINT16 now = (0x1FFF & micros()) << 3;//init
            TMM_UINT16 expire = this->_expire;
            TMM_UINT16 d = expire - now;
            if (d >= 0x7FFFF) {
                //expired
                this->_expire = next;
                return;
            }
            while (true) {
                TMM_UINT16 now = (0x1FFF & micros()) << 3;//init
                if ((TMM_UINT16)(expire - now) > d) {
                    this->_expire = next;
                    return;
                }
            }
        }
    };




    template <typename T> class IPyIterator
    {
    public:
        virtual ~IPyIterator() {}
        virtual T next() = 0;
        virtual bool hasNext()const = 0;
    };
    class IBitIterator:public IPyIterator<TMM_UINT8> {
    };


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
        bool hasNext()const {
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
    template <size_t SIZE> class RawBits :public BitArray {
    protected:
        TMM_UINT8 b[SIZE];
    protected:
        inline const TMM_UINT8* refBuf()const { return this->b; };
    public:
        RawBits() :BitArray(SIZE) {}
        void set(int i, int v) {
            auto n = (v & 0x01) << (7 - i % 8);
            this->b[i / 8] = (n == 0) ? (this->b[i / 8] & (~n)) : (this->b[i / 8] | n);
        }
    };

    template <size_t SIZE> class PnBits :public RawBits<SIZE> {
    public:
        PnBits() :RawBits<SIZE>() {}
        void setRandom(TMM_UINT16 seed) {
            randomSeed(seed);
            for (auto i = 0;i < (SIZE + 7) / 8;i++) {
                this->b[i] = random(255);
            }
        }
    };
    class DataBits :public IPyIterator<TMM_UINT8>
    {
    private:
        TMM_UINT16 _c;
        const TMM_UINT8* _data;
        TMM_UINT8 _lastbit;
    private:
        TMM_UINT16 size;
    public:
        DataBits() :_c{ 0 }, _data{ NULL }, _lastbit{ 0 }, size{0} {}

        void init(const TMM_UINT8* data, size_t size) {
            this->_data = data;
            this->size = (TMM_UINT16)(size);
            this->_lastbit = 0;
            this->_c = 0;
        }
        bool hasNext()const {
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

    /** Preambleビット列のイテレータです。
     */
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
        bool hasNext()const {
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
            _tval{ 0 }, _co{ 0 }, _n{ 0 }, _tone{NULL},_preamble(PreambleIter(preamble_cycle))
        {
        }
        void init(BitArray* ref_tone,const TMM_UINT8* data, size_t size) {
            this->_tone = ref_tone;
            this->_preamble.init();
            this->_data.init(data, size);
            this->_co = 0;
            this->_n = 0;
        }
        bool hasNext()const {
            return this->_co < 4;
        }
        TMM_UINT8 next()
        {
            const auto TONE_SIZE=this->_tone->size;
            for (;;) {
                switch (this->_co) {
                case 0:
                    if (this->_n >= TONE_SIZE) {
                        this->_co++;
                        this->_tval = this->_preamble.next();//reset
                        this->_tone->reset();
                        continue;
                    }
                    this->_n++;
                    return (this->_n % 3) & 0x01;
                case 1:
                    if (!this->_tone->hasNext()) {
                        if (!this->_preamble.hasNext()) {
                            this->_co++;
                            continue;
                        }
                        this->_tone->reset();
                        this->_tval = this->_preamble.next();
                    }
                    return this->_tval ^ this->_tone->next();
                case 2:
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
                case 3:
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
        TbskPulseModulator(int preamble_cycle=4,int seed = 299) :_pulse{PulseIter(preamble_cycle)} {
            this->_pn.setRandom((TMM_INT16)seed);
        }
        IBitIterator& modulate(const char src[],int srclen=-1) {
            this->_pulse.init(&this->_pn, (const TMM_UINT8*)src, srclen<0?strlen(src)*8:srclen);
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


