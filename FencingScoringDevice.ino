#define DECODE_NEC
#include <IRremote.hpp>

int RECV_PIN = 7;

void setup(){     
  Serial.begin(9600);     
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);    
}    
void loop(){ 
  if (IrReceiver.decode()){    
   Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);    
   IrReceiver.resume();   
  }     
}
