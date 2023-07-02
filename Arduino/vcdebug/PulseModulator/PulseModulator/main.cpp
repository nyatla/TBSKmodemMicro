#include "../../../src/TbskModemMicro.h"
#include "../../../src/classes/PreambleDetector.h"
#include "../../../src/classes/AverageInterator16.h"
#include "../../../src/classes/TraitBlockDecoder.h"



#include <math.h>

using namespace TBSKmodemMicro;

/*
class TbskSocket {
	bool ready();
	/// <summary>
	/// 0..255	受信したデータ,-1 受信データなし
	/// </summary>
	/// <returns></returns>
	int recv();
	int recv(int& n);
	int recv(void* buf,int buf_len);
	void send(void* buf, int buf_len);
	void isr();
	void interrupt();
};

*/

#include <vector>
#define TONE_SIZE_	100

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

	TbskDemodulator<TONE_SIZE_> dem;
	BufIterator src(src2.data(), (TMM_INT16)src2.size());
	TMM_UINT8 dest[10] = {};
	int r = dem.demodulate(src, dest, 10);
	int r2 = -1;
	while (r2 == -1) {
		r2 = dem.read(1, dest, 10, 1000);
	}


	return 0;

}

