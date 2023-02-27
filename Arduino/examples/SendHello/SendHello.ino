// This sketch sends a 16kHz-160bps TBSK modulated message from
// a device connected to the GPIO output. It can be received by
// the TBSKmodem receiver. https://nyatla.jp/tbskmodem/
#include "TbskModemMicro.h"
using namespace TBSKmodemMicro;

#define LED_PIN 25
#define SPK_PIN 2
TbskPulseModulator<100> tpm;
void setup() {
  pinMode(LED_PIN, OUTPUT);  //LED
  pinMode(SPK_PIN, OUTPUT);   //OutputPin
}
void loop()
{
  digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  tpm.write(SPK_PIN,16000,"Hello arduino");  //send message via GPIO pulse audio.
  digitalWrite(LED_PIN, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(1000);

}
