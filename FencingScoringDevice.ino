/*
   The body cord has 3 prongs: A, B, and C.
   A and B are closer together than B and C.
   C is always connected to ground.
   A is connected to the fencer's jacket
   In foil, the A line is connected to the lamé and the B line runs up a wire to the tip of the weapon.
   The B line is normally connected to the C line through the tip.

   Foil:
   When the tip is depressed, the circuit is broken and one of three things can happen:
      The tip is touching the opponent's lamé (their A line): Valid touch
      The tip is touching the opponent's weapon or the grounded strip: nothing, as the current is still flowing to the C line.
      The tip is not touching either of the above: Off-target hit (white or yellow light).
  The apparatus registers when a break occurs in the circuit of the foil, that is to say that the
  electrical current which is circulating permanently in the foil circuit is broken when a
  touch is made.

  B is hot, A and C are ground. If B connects to opponent's A, a point is scored. If B disconnects from C but isn't connecting to A, off-target light.

  Epee
  In Épée, the A and B lines run up separate wires to the tip (there is no lamé).
  When the tip is depressed, it connects the A and B lines, resulting in a valid touch.
  However, if the tip is touching the opponents weapon (their C line) or the grounded strip,
  nothing happens when it is depressed, as the current is redirected to the C line.
  Grounded strips are particularly important in Épée, as without one, a touch to the floor registers as a valid touch
  The apparatus registers when contact is established between the wires forming the circuit in
  the épée, thus completing the circuit.

  B is hot



  Sabre:
  In Sabre, similarly to Foil, the A line is connected to the lamé, but both the B and C lines are connected to the body of the weapon.
  Any contact between the B/C line (doesn't matter which, as they are always connected) and the opponent's A line (their lamé) results in a valid touch.
  There is no need for grounded strips in Sabre, as hitting something other than the opponent's lame does nothing.
  The apparatus works by contact between any uninsulated part of the sabre and the
  conductive surface of the opposing fencer’s jacket, glove and mask.


  http://www.columbusfencing.org/armourer.htm





*/

#define USE_TIMER_1 true
#define DECODE_NEC
#include <IRremote.hpp>
#include <SoftwareSerial.h>
#include "TimerInterrupt.h"
#define TIMER_INTERVAL_MS 1000

#define IR_RECV 7

#define FENCER1  4
#define FENCER2  5
#define GREEN 8
#define RED 9
#define WHITE1 10
#define WHITE2 11
#define SCORE1 12
#define SCORE2 13
#define BUZZER 14

int currentTime = 0;

struct State {
  int score1 = 0;
  int score2 = 0;
  String mode = "foil";
  boolean muted = false;
  boolean buzzerOn = false;
  int buzzerStartTime = 0;
} state;

SoftwareSerial hc06(2, 3); //Tx=2, Rx=3

void TimerHandler() {
  if (currentTime > 0) {
    currentTime -= 1;
    writeTime();
  }
}
void writeScore(int player, int score){
  //write to either SCORE1 or SCORE2 with state.score1/2
}
void writeTime(){
  //write currentTime to the timer LEDs
}
void soundBuzzer(){
  if (state.muted == false){
    digitalWrite(BUZZER, HIGH);   
    state.buzzerOn = true;
    state.buzzerStartTime = millis(); 
  }
}
void parseCommand(String cmd) {
  if (cmd == "inc1\n" || cmd == "f708ff00") {
    state.score1 += 1;
    //write new score to SCORE1
    Serial.println("inc1");
    if (cmd == "f708ff00") {
      hc06.write("inc1\n");
    }
  }
  else if (cmd == "inc2\n" || cmd == "a55aff00") {
    state.score2 += 1;
    //write new score to SCORE2
    Serial.println("inc2");
    if (cmd == "a55aff00") {
      hc06.write("inc2\n");
    }
  }
  else if (cmd == "dec1\n" || cmd == "bd42ff00") {
    if (state.score1 > 0) {
      state.score1 -= 1;
      //write new score to SCORE1
    }
    Serial.println("dec1");
    if (cmd == "bd42ff00") {
      hc06.write("dec1\n");
    }
  }
  else if (cmd == "dec2\n" || cmd == "b54aff00") {
    if (state.score2 > 0) {
      state.score2 -= 1;
      //write new score to SCORE2
    }
    Serial.println("dec2");
    if (cmd == "b54aff00") {
      hc06.write("dec2\n");
    }
  }
  else if (cmd == "mute\n" || cmd == "b847ff00") {
    if (state.muted == 1) {
      state.muted == 0;
    }
    else {
      state.muted == 1;
    }
    Serial.println("mute");
  }

  else if (cmd == "timer1\n" || cmd == "f30cff00") {
    currentTime = 60;
    Serial.println("timer1");
    if (cmd == "f30cff00") {
      hc06.write("timer1\n");
    }
  }
  else if (cmd == "timer3\n" || cmd == "a15eff00") {
    currentTime = 180;
    Serial.println("timer3");
    if (cmd == "a15eff00") {
      hc06.write("timer3\n");
    }
  }
  else if (cmd == "foil\n") {
    state.mode = "foil";
    Serial.println("foil");
  }
  else if (cmd == "epee\n") {
    state.mode = "epee";
    Serial.println("epee");
  }
  else if (cmd == "sabre\n") {
    state.mode = "sabre";
    Serial.println("sabre");
  }
  else if (cmd == "mode\n" || cmd == "b946ff00") {
    Serial.println("mode");
    if (state.mode == "foil") {
      state.mode = "epee";
    }
    else if (state.mode == "epee") {
      state.mode = "sabre";
    }
    else {
      state.mode = "foil";
    }
    if (cmd == "b946ff00") {
      hc06.write("mode\n");
    }
  }
}

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(WHITE1, OUTPUT);
  pinMode(WHITE2, OUTPUT);
  
  Serial.begin(9600);
  hc06.begin(9600);
  IrReceiver.begin(IR_RECV, ENABLE_LED_FEEDBACK);
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS, TimerHandler);
}
void loop() {
  //Check if the buzzer should be turned off yet, 1 second must have elapsed
  if (state.buzzerOn == true){ 
    if (millis() -1000 > state.buzzerStartTime){
      digitalWrite(BUZZER, LOW);
      state.buzzerOn = false;
    }
  }
  //Check if there is any IR data to decode
  if (IrReceiver.decode()) {
    String cmd(IrReceiver.decodedIRData.decodedRawData, HEX);
    parseCommand(cmd);
    Serial.println(cmd);
    //
    IrReceiver.resume();
  }
  //Check if there is any bluetooth data to decode
  if (hc06.available()) {
    char buffer[10];
    int index = 0;
    while (index < 10) {
      if (hc06.available()) {
        buffer[index] = hc06.read();
        if (buffer[index] == '\n') { //end of message
          char cmd[index + 2];
          for (int i = 0; i <= index; i++) {
            cmd[i] = buffer[i];
          }
          cmd[index + 1] = '\0'; //cmd is a char array with a \n and \0 as the last 2 chars, to make it a string
          parseCommand(cmd);
          index = 10;//exit
        }
        else {
          index++;
        }
      }
    }
  }
}
