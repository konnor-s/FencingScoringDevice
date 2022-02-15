/*
 * The body cord has 3 prongs: A, B, and C.
 * A and B are closer together than B and C.
 * C is always connected to ground. 
 * A is connected to the fencer's jacket
 * In foil, the A line is connected to the lamé and the B line runs up a wire to the tip of the weapon. 
 * The B line is normally connected to the C line through the tip. 
 * 
 * Foil:
 * When the tip is depressed, the circuit is broken and one of three things can happen:
      The tip is touching the opponent's lamé (their A line): Valid touch
      The tip is touching the opponent's weapon or the grounded strip: nothing, as the current is still flowing to the C line.
      The tip is not touching either of the above: Off-target hit (white or yellow light).
 *The apparatus registers when a break occurs in the circuit of the foil, that is to say that the
  electrical current which is circulating permanently in the foil circuit is broken when a
  touch is made.
 *
 *B is hot, A and C are ground. If B connects to opponent's A, a point is scored. If B disconnects from C but isn't connecting to A, off-target light.
 *
 *Epee
 *In Épée, the A and B lines run up separate wires to the tip (there is no lamé). 
 *When the tip is depressed, it connects the A and B lines, resulting in a valid touch. 
 *However, if the tip is touching the opponents weapon (their C line) or the grounded strip, 
 *nothing happens when it is depressed, as the current is redirected to the C line. 
 *Grounded strips are particularly important in Épée, as without one, a touch to the floor registers as a valid touch
 *The apparatus registers when contact is established between the wires forming the circuit in
  the épée, thus completing the circuit.
 *
 *B is hot
 *
 *
 *
 *Sabre:
 *In Sabre, similarly to Foil, the A line is connected to the lamé, but both the B and C lines are connected to the body of the weapon. 
 *Any contact between the B/C line (doesn't matter which, as they are always connected) and the opponent's A line (their lamé) results in a valid touch. 
 *There is no need for grounded strips in Sabre, as hitting something other than the opponent's lame does nothing.
 *The apparatus works by contact between any uninsulated part of the sabre and the
  conductive surface of the opposing fencer’s jacket, glove and mask.
 *
 *
 *http://www.columbusfencing.org/armourer.htm
 *
 *
 *
 *
 *
  */

#define DECODE_NEC
#include <IRremote.hpp>
#include <SoftwareSerial.h>

int IR_RECV = 7;

int YELLOW1 = 23;
int RED1 = 24;
int BLACK1 = 25;
int YELLOW2 = 26;
int RED2 = 27;
int BLACK2 = 28;

int VOLUME = 19; //if we need an ADC input from the potentiometer to control the speaker volume
int FENCER1 = 4;
int FENCER2 = 5;

SoftwareSerial hc06(2,3); //Tx=2, Rx=3

void setup(){    
  Serial.begin(9600);      
  hc06.begin(9600);
  IrReceiver.begin(IR_RECV, ENABLE_LED_FEEDBACK);    
}    
void loop(){ 
  if (IrReceiver.decode()){ 
    //IrReceiver.printIRResultShort(&Serial);
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    if (IrReceiver.decodedIRData.decodedRawData == 0xBA45FF00){
      hc06.write("Hello from your Arduino!\n");
      Serial.println("message sent");
    }
     //
     IrReceiver.resume();   
  }   
   
  if (hc06.available()){
    Serial.write(hc06.read());
  }
  if (Serial.available()){
    hc06.write(Serial.read());
  }   
}
