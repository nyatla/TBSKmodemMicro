// TbskDemodulator test.
//This sketch modulate text to TBSK signal and demodulate to text.
// the TBSKmodem receiver. https://nyatla.jp/tbskmodem/
#include "TbskModemMicro.h"
using namespace TBSKmodemMicro;

#define LED_PIN 25

void setup() {
  pinMode(LED_PIN, OUTPUT);  //LED
}
void loop()
{
  TbskPulseModulator<100> tpm;
	auto& src1=tpm.modulate("hello",1);
	std::vector<TMM_INT16> src2;
	for (auto i = 0;i < 100;i++) { src1.next(); }
	while (src1.hasNext()) {
		float v= ((float)src1.next()-.5f)*2;
		v = v > 0 ? v * 32767 : v * 32768;
		src2.push_back((TMM_INT16)v);
	}
	TbskDemodulator<100> dem;
	BufIterator src(src2.data(), (TMM_INT16)src2.size());
	TMM_UINT8 dest[10] = {};
	int r = dem.demodulate(src, dest, 10);
  Serial.println(r);
  //modulate and demodulate on ideal.
  return;
}
