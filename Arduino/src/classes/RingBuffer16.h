#pragma once
#include "./utils.h"

#include <vector>
#include <memory>
namespace TBSKmodemMicro
{
    template <typename T, TMM_UINT16 SIZE> class RingBuffer16;

    template <typename T> class RefIterator :public IPyIterator<T>
    {
    private:
        TMM_UINT16 _size;
        TMM_UINT16 _pos;
        const T* _buf;
        TMM_UINT16 _buf_len;
    public:
        RefIterator() :_pos{ 0 }, _size{ 0 }, _buf{ NULL },_buf_len{0}
        {
        }
        T next()override {
            TBSK_ASSERT(this->_size > 0);
            this->_size = this->_size - 1;
            TMM_UINT16 p = this->_pos;
            this->_pos = (this->_pos + 1) % this->_buf_len;
            return this->_buf[p];

        };
        bool hasNext() override {
            return this->_size > 0;
        };
        void reset(const T* buf, TMM_UINT16 buf_len, TMM_UINT16 pos, TMM_UINT16 size) {
            this->_buf = buf;
            this->_buf_len = buf_len;
            this->_pos = pos;
            this->_size = size;
        }

    };


    /// <summary>
    /// 格納数に16bit制限があるRingBuffer
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <typeparam name="SIZE"></typeparam>
    template <typename T, TMM_UINT16 SIZE> class RingBuffer16
    {
    protected:
        T _buf[SIZE];
    private:
        TMM_UINT16 _p;
    public:


    public:
        RingBuffer16(const T pad) :_p{ 0 }
        {
            auto L = this->getLength();
            for (auto i = 0;i < L;i++) {
                this->_buf[i] = pad;
            }
        }
        virtual ~RingBuffer16() {
        }
    public:

        T append(T v) {
            auto& b = this->_buf;
            const auto length = SIZE;
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
        IPyIterator<T>& subIter( TMM_UINT16 pos, TMM_UINT16 size, RefIterator<T>& out)const
        {
            out.reset(this->_buf,SIZE, (pos + this->_p) % SIZE, size);
            return out;
        }

        T getTail()const {
            auto length = SIZE;
            return this->_buf[(this->_p - 1 + length) % length];

        }
        T getTop()const {
            return this->_buf[this->_p];
        }
        TMM_UINT16 getLength()const {
            return SIZE;
        }
    };



}