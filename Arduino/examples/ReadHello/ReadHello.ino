// This sketch receives a 16kHz-160bps TBSK signal from AD0 pin.
// TBSK signal generator in here. https://nyatla.jp/tbskmodem/
#include "TbskModemMicro.h"
using namespace TBSKmodemMicro;

TbskDemodulator<100> dem;
#define LED_PIN 25
#define AD_PIN 26
void setup() {
  pinMode(LED_PIN, OUTPUT);  //LED
  analogReadResolution(12);
}
void loop()
{
  int r=-1;
	TMM_UINT8 dest[10] = {};  
  digitalWrite (LED_PIN,HIGH);
  Serial.println("GO");

  while(r==-1){
     r=dem.read(AD_PIN,16000,dest,10,5000);
  }
  digitalWrite (LED_PIN,LOW);
  for(auto i=0;i<r;i++){
  }
  //d has received data.
  delay(1000);

  return;
}
