// This is single-pin echo back test.
// Connect "CLIPPED" piezo speaker to AD0 pin.
// Send a signal near the speaker and RasPico will send back the message.
// the TBSKmodem transmitter. https://nyatla.jp/tbskmodem/
#include "TbskModemMicro.h"
using namespace TBSKmodemMicro;

TbskPulseModulator<100> tpm;
TbskDemodulator<100> dem;

#define LED_PIN 25
#define AD_PIN 26
#define SP_PIN 26

void setup() {
  pinMode(LED_PIN, OUTPUT);  //LED
}

void loop()
{
  int r=-1;
	TMM_UINT8 recv[10] = {};  
  digitalWrite (LED_PIN,HIGH);
  analogReadResolution(12);

  while(r==-1){
     r=dem.read(AD_PIN,16000,recv,10,5000);
  }
  digitalWrite (LED_PIN,LOW);
  delay(1000);
  //send back data
  TMM_UINT8 send[16]={};
  memcpy(send,"echo:",5);
  memcpy(send+5,recv,r);
  pinMode(SP_PIN, OUTPUT);
  tpm.write(SP_PIN,16000,(const char*)send,r+5);

  return;
}
