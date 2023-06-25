#pragma once
#include "./PreambleDetector.h"
#include "./AverageInterator16.h"


/*



*/

#include <math.h>

using namespace TBSKmodemMicro;

namespace TBSKmodemMicro
{


    template <TMM_UINT16 TRAIT_BLOCK_TICKS> class TraitBlockDecoder :public IPyIterator<TMM_INT16>
    {
    private:
        //AverageInterator16 < (TRAIT_BLOCK_TICKS / 10) < 2 ? 2 : TRAIT_BLOCK_TICKS / 10) > _avefilter;
        AverageInterator16 <TRAIT_BLOCK_TICKS / 10> _avefilter;
        TMM_INT16 _threshold;
        TMM_INT16 _last_data;
        FF16SelfCorrcoefIterator<TRAIT_BLOCK_TICKS, TRAIT_BLOCK_TICKS> _cof;
        IPyIterator<TMM_INT16>& _src;
        TMM_INT16 _shift;
        TMM_UINT32 _pos;
        TMM_INT16 _state;   //0:hasNextコール待ち,1:nextコール待ち,2:終端到達
        TMM_INT16 _last_next;
        TMM_INT16 _preload_size;
        TMM_INT16 _block_skip_size;
        TMM_INT16 _block_skip_counter;
    public:

        TraitBlockDecoder(IPyIterator<TMM_INT16>& src, float threshold) :
            _threshold(FLOAT_TO_FF16(threshold)),
            _last_data(0),
            _cof(FF16SelfCorrcoefIterator<TRAIT_BLOCK_TICKS, TRAIT_BLOCK_TICKS>()),
            _avefilter(AverageInterator16 <TRAIT_BLOCK_TICKS / 10>(_cof)),
            _src(src),
            _shift(0),
            _pos(0),
            _state(0),
            _last_next(0),
            _preload_size(TRAIT_BLOCK_TICKS + (TRAIT_BLOCK_TICKS / 10) / 2 - 1),
            _block_skip_size(TRAIT_BLOCK_TICKS - 1 - 2),//#スキップ数
            _block_skip_counter(TRAIT_BLOCK_TICKS) //#スキップ数
        {
            TBSK_ASSERT(TRAIT_BLOCK_TICKS / 10 > 2);
            this->_cof.init(src);
        }




        TMM_INT16 next()
        {
            TBSK_ASSERT(this->_state == 1);
            switch (this->_state)
            {
            case 1:
                this->_state = 0;
                return this->_last_next;

            case 2:
            default:
                return -1;//OMG
            }
        }



        /// <summary>
        /// 次回のnextが成功するかを返す。
        /// hasNextがtrueを返した場合、次回のhasNextが呼び出される前に、nextを呼び出さなければならない。
        /// </summary>
        /// <returns></returns>
        bool hasNext()
        {
            switch (this->_state)
            {
            case 1:return true;//next待ちならばtrueである。
            case 2:return false;//終端到達ならfalse
            }
            auto& lavefilter = this->_avefilter;
            //#平均イテレータの初期化(初めの一回目だけ)
            while (this->_preload_size > 0) {
                if (!lavefilter.hasNext()) {
                    //終端
                    this->_state = 2;//closed;
                    return false;
                }
                lavefilter.next();
                this->_preload_size--;
            }
            //#ブロックヘッダの読み出し(1ブロック読出しごとにリセット)
            while (this->_block_skip_counter > 0) {
                if (!lavefilter.hasNext()) {
                    //終端
                    this->_state = 2;//closed;
                    return false;
                }
                lavefilter.next();
                this->_block_skip_counter = this->_block_skip_counter - 1;
            }

            TMM_INT32 samples[3];
            for (size_t i = 0;i < 3;i++) {
                if (!lavefilter.hasNext()) {
                    //終端
                    this->_state = 2;//closed;
                    return false;
                }
                samples[i] = lavefilter.next();
            }


            TMM_INT32 r = samples[1];
            if (samples[0] * samples[1] < 0 or samples[1] * samples[2] < 0) {
                //#全ての相関値が同じ符号でなければ何もしない
                this->_block_skip_counter = this->_block_skip_size; //#リセット
            }
            else {
                //#全ての相関値が同じ符号
                TMM_INT32 asamples[3];
                for (size_t i = 0;i < 3;i++) {
                    asamples[i] = abs(samples[i]);
                }
                //#一番大きかったインデクスを探す
                if (asamples[1] > asamples[0] and asamples[1] > asamples[2]) {
                    //#遅れも進みもしてない
                }
                else if (asamples[0] > asamples[2]) {
                    //#探索場所が先行してる
                    this->_shift = this->_shift - 1;
                }
                else if (asamples[0] < asamples[2]) {
                    //#探索場所が遅行してる
                    this->_shift = this->_shift + 1;
                }
                else {
                    //#不明
                }

                if (this->_shift > 10) {
                    this->_shift = 0;
                    //# print(1)
                    this->_block_skip_counter = this->_block_skip_size + 1;
                }
                else if (this->_shift < -10) {
                    this->_shift = 0;
                    //# print(-1)
                    this->_block_skip_counter = this->_block_skip_size - 1;
                }
                else {
                    this->_block_skip_counter = this->_block_skip_size;
                }

            }



            //# print(self._src.pos, r);
            TMM_INT16 th = this->_threshold;
            this->_pos = this->_pos + 1;
            if (r > th) {
                //# print(1, 1)
                this->_last_data = r;
                this->_last_next = 1;
                this->_state = 1;//next待ち
                return true;
                //return this->getResultAsint(1, (this->_src.pos, r, this->_trait_block_ticks));
            }
            else if (r < -th) {
                //# print(2,0)
                this->_last_data = r;
                this->_last_next = 0;
                this->_state = 1;//next待ち
                return true;
                //return this->getResultAsint(0, (this->_src.pos, r, this->_trait_block_ticks));
            }
            else if (this->_last_data - r > th) {
                //# print(3,1)
                this->_last_data = r;
                this->_last_next = 1;
                this->_state = 1;//next待ち
                return true;
                //return this->getResultAsint(1, (this->_src.pos, r, this->_trait_block_ticks))
            }
            else if (r - this->_last_data > th) {
                //# print(4,0)
                this->_last_data = r;
                this->_last_next = 0;
                this->_state = 1;//next待ち
                return true;
                //return this->getResultAsint(0, (this->_src.pos, r, this->_trait_block_ticks));
            }
            else {
                this->_state = 2;//closed;
                return false;
            }
        }
    };

}