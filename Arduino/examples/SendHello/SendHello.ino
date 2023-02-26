// This sketch sends a 16kHz-160bps TBSK modulated message from
// a device connected to the GPIO output. It can be received by
// the TBSKmodem receiver. https://nyatla.jp/tbskmodem/

#include "data/TbskModemMicro.h"
using namespace TBSKmodemMicro;
TbskPulseModulator<100> tpm;
void setup() {
  pinMode(25, OUTPUT);  //LED
  pinMode(2, OUTPUT);   //OutputPin
}

void loop()
{

  digitalWrite(25, HIGH);  // turn the LED on (HIGH is the voltage level)
  tpm.write(2,16000,"Hello arduino");  //send message via GPIO pulse audio.
  digitalWrite(25, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(1000);

}
