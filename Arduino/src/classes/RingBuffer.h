#pragma once
#include "./utils.h"

#include "./RingBuffer.h"
#include <vector>
#include <memory>
namespace TBSKmodemMicro
{

    /// <summary>
    /// 格納数に16bit制限があるRingBuffer
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <typeparam name="SIZE"></typeparam>
    template <typename T, TMM_UINT16 SIZE> class RingBuffer16
    {
    private:
        T _buf[SIZE];
        TMM_UINT16 _p;
    public:
        class SubIterator :public IPyIterator<T>
        {
        private:
            TMM_UINT16 _size;
            TMM_UINT16 _pos;
            const RingBuffer* _buf;
        public:
            SubIterator() :_pos{0}, _size{0}, _buf{nullptr}
            {
            }
            reset(const RingBuffer& me, TMM_UINT16 pos, TMM_UINT16 size) {
                this->_buf = &me;
                this->_pos = pos;
                this->_size = size;
            }
            T next()override {
                TBSK_ASSERT(this->_size > 0);
                this->_size = this->_size - 1;
                TMM_UINT16 p = this->_pos;
                this->_pos = (this->_pos + 1) % this->_buf->getLength();
                return this->_buf.at(p);

            };
            bool hasNext()const override {
                return this->_size > 0;
            };
        };

    public:
        RingBuffer16(const T pad) :_p{0}
        {
            auto L = this->getLength();
            for (auto i = 0;i<L;i++) {
                this->_buf = pad;
            }
        }
        virtual ~RingBuffer16() {
        }
    public:

        T append(T v) {
            auto& b = this->_buf;
            const auto length = b.size();
            auto ret = b[this->_p];
            b[this->_p] = v;
            this->_p = (this->_p + 1) % length;
            return ret;
        }
        void extend(const T* buf, int len) {
            for (auto i = 0;i < len;i++) {
                this->append(*(buf + i));
            }
        }
        /// <summary>
        /// この関数は、posからsizeの要素を参照するイテレータを返します。
        /// イテレータの生存期間はこのインスタンスと同じです。
        /// バッファ操作を行う関数を使用すると、イテレータの返り値は影響を受けます。
        /// </summary>
        /// <param name="pos"></param>
        /// <param name="size"></param>
        /// <returns></returns>
        void SubIter(SubIterator& out,TMM_UINT16 pos, TMM_UINT16 size)const
        {
            out.reset(*this,(pos+this->_p)%this->getLength(), size);
        }

        T getTail()const {
            auto length = this->_buf.size();
            return this->_buf[(this->_p - 1 + length) % length];

        }
        T getTop()const {
            return this->_buf[this->_p];
        }
        TMM_UINT16 getLength()const {
            return this->_buf.size();
        }
    };


    //最大値が256個で死ぬので注意
    template <TMM_UINT16 LENGTH> class SumIterator16:public IPyIterator<TMM_INT16>
    {
    protected:
        IPyIterator<TMM_INT8> _src;
        TMM_INT16 _sum;
        RingBuffer16<TMM_INT8, LENGTH> _buf;

    public:
        SumIterator16(IPyIterator<TMM_INT8>& src) :_src{ src }, _sum{ 0 }, _buf{0} {
        }

        TMM_INT16 next()override {
            auto s = this->_src.next();
            auto d = this->_buf.append(s);
            this->_sum = this->_sum + s - d;
            return this->_sum;
        };
        TMM_INT16 hasNext()override const {
            return this->_src.next();
        };
        const RingBuffer16& getBuf() const {
            return this->_buf;
        }
    };

    template <TMM_UINT16 LENGTH> class AverageInterator16 :public SumIterator16<LENGTH>
    {
    private:
        // ticks個の入力値の平均値を返します。
        //このイテレータはRecoverableStopInterationを利用できます。
    public:
        AverageInterator16(IPyIterator<TMM_INT8 >> &src, int ticks):SumIterator16(src)
        {
            TMM_INT16 next()override {
                return SumIterator16<LENGTH>::next() / this->_buf.getLength();
            };
            TMM_INT16 hasNext()override {
                return SumIterator16<LENGTH>::hasNext();
            };
        }

    };

    
}
