#include "../../../src/TbskModemMicro.h"
#include "../../../src/classes/PreambleDetector.h"
#include "../../../src/classes/AverageInterator16.h"
#include "../../../src/classes/TraitBlockDecoder.h"



#include <math.h>

using namespace TBSKmodemMicro;



/// <summary>
/// TMM_INT16配列を返す
/// </summary>
class BufIterator :public IPyIterator<TMM_INT16>
{
private:
	const TMM_INT16* _buf;
	const TMM_UINT16 _size;
	TMM_UINT16 _pos;
public:
	BufIterator(const TMM_INT16* buf, TMM_UINT16 size):_buf(buf),_size(size),_pos(0)
	{
	}
	TMM_INT16 next(){
		TBSK_ASSERT(this->hasNext());
		auto p = this->_pos;
		this->_pos++;
		return this->_buf[p];
	};
	bool hasNext() {
		return this->_pos < this->_size;
	};
};








#include <vector>







#define TONE_SIZE_	100

/*
int ff16cpfftest_main()
{
	TbskPulseModulator<TONE_SIZE> mod;
	auto& src1 = mod.modulate("hello", 0);
	std::vector<TMM_INT16> src2;
	for (auto i = 0;i < 100;i++) { src1.next(); }
	while (src1.hasNext()) {
		float v = ((float)src1.next() - .5f) * 2;
		v = v > 0 ? v * 32767 : v * 32768;
		src2.push_back((TMM_INT16)v);
	}
	//for (int i = 0;i < 1000;i++) {
	//	src2.push_back((i%100)*300);
	//}

	BufIterator src(src2.data(), (TMM_INT16)src2.size());
	FF16SelfCorrcoefIterator<TONE_SIZE, TONE_SIZE> _cof;
	_cof.init(src);
	int i = 0;
	while (_cof.hasNext()) {
		auto t = _cof.next();
		printf("%4d,%d\n", i, t);
		i++;
	}
	////////
	//FF16SelfCorrcoefIterator<10, 10> a;
	//AveLog<10, 10> b;
	//TickLog<10> c;
	////////
	//CoffPreambleDetector<4, TONE_SIZE> d(src,1.0f);
	//d.hasNext();

	return 0;

}
*/
template <TMM_INT16 TONE_SIZE> class TbskDemodulator
{
	TMM_INT32 _detected_pos;
	TraitBlockDecoder<TONE_SIZE_> bd(src, 0.2f);
	bool waitForSignal(TMM_INT16 carrier)
	{
		//timeoutの間だけ有効なIterator
		TMM_INT16 pd_pos = 0;
		CoffPreambleDetector<4, TONE_SIZE> pd(src, 1.0f);
		if (!pd.hasNext()) {
			return false;
		}
		this._detected_pos = pd.next();
		return true;
	}
	/// <summary>
	/// 0,1、終端で-1
	/// </summary>
	TMM_INT16 readBit()
	{
		if (bd.hasNext()) {
			return bd.next();
		}
		return -1;
	}
	/// <summary>
	/// 0-255、終端で-1
	/// </summary>
	TMM_INT16 read()
	{
		TMM_UINT8 w = 0;
		for (size_t i = 0;i < 8;i++) {
			if (!bd.hasNext()) {
				return -1;
			}
			w = w | db.next();
		}
		return (TMM_INT16)w;
	}
};


int main()
{

	TbskPulseModulator<TONE_SIZE_> mod;
	auto& src1=mod.modulate("hello",1);
	std::vector<TMM_INT16> src2;
	for (auto i = 0;i < 100;i++) { src1.next(); }
	while (src1.hasNext()) {
		float v= ((float)src1.next()-.5f)*2;
		v = v > 0 ? v * 32767 : v * 32768;
		src2.push_back((TMM_INT16)v);
	}

	BufIterator src(src2.data(),(TMM_INT16)src2.size());

	////////
    TMM_INT16 pd_pos = 0;
    {
        CoffPreambleDetector<4, TONE_SIZE_> pd(src, 1.0f);
        if (!pd.hasNext()) {
            return 0;
        }
        pd_pos=pd.next();
    }
    {
        for (size_t i = 0;i < TONE_SIZE_ + pd_pos;i++) {
            if (!src.hasNext()) {
                return 0;
            }
            src.next();
        }
        TraitBlockDecoder<TONE_SIZE_> bd(src, 0.2f);
        while (bd.hasNext()) {
            printf("%d\n", bd.next());
        }

    }

	return 0;

}

