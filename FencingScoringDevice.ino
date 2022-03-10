/*
  The body cord has 3 prongs: A, B, and C.
  A and B are closer together than B and C.
  C is always connected to ground.

  Table of analog voltage values for valid and off-target hits
  ///////////////////////////////////
  //            A    B      C
  // Foil:
  // Normal     0    510    510
  // Valid      510  510    0
  // Off-target 0    1023   0
  //
  // Epee:
  // Normal     0    1023   0
  // Valid      510  510    0
  // Off-target 330  330    330
  //
  // Sabre:
  // Normal     0    510    510
  // Valid      330  330    330
  /////////////////////////////////////

*/
//Test COmment 123

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#define USE_TIMER_1 true
#define DECODE_NEC
#include <IRremote.hpp>
#include <SoftwareSerial.h>
#include "TimerInterrupt.h"
#define TIMER_INTERVAL_MS 1000

#define HC06_RX 3
#define HC06_TX 2
#define IR_RECV 4

#define MULT_0 5
#define MULT_1 6 
#define MULT_2 7
#define MULT_3 8 
#define INHIBIT_1 9  
#define INHIBIT_2 10
#define DATA_1 11
#define DATA_2 12

#define BUZZER 13
#define WEAPON_1_PINC A0   // Ground A pin - Analog
#define WEAPON_1_PINA A0 // Lame   A pin - Analog (Epee return path)
#define WEAPON_1_PINB A1  // Weapon A pin - Analog
#define WEAPON_2_PINC A3  // Ground B pin - Analog
#define WEAPON_2_PINA A2  // Lame   B pin - Analog (Epee return path)
#define WEAPON_2_PINB A3  // Weapon B pin - Analog

#define FOIL 0
#define EPEE 1
#define SABRE 2

Adafruit_7segment matrix = Adafruit_7segment();

//depress time is the amount of time needed to maintain valid contact for a hit to count
const int depress [] = {300000, 45000, 120000}; //foil, epee, sabre
//lockout is the amount of time after a hit to allow the other person to get a hit, before the round ends
const int lockout [] = {14000, 2000, 1000};
const int validVoltage [] = {510, 510, 330}; //its the same voltage for pin A and B on a valid hit
const int offTargetVoltageA [] = {88, 330, 2000}; //sabre doesn't have off target
const int offTargetVoltageB [] = {950, 330, 2000};
//fudgi0 to 88 and 1023 to 950 since it checks +-89


#define BUZZER_TIME 2000 //buzzer for 2 seconds
#define LIGHT_TIME 2000 //lights stay on for 2 additional seconds

int i = 0;
int currentTime = 0;

struct State {
  int score1 = 0;
  int score2 = 0;
  int mode = FOIL;
  boolean muted = false;
  boolean buzzerOn = false;
  long buzzerStartTime = 0;
  boolean inAction = false;
};

struct WeaponState {
  long depressTime1 = 0;
  long depressTime2 = 0;
  bool lockedOut = false;
  int weapon1_B = 0;
  int weapon2_B = 0;
  int weapon1_A = 0;
  int weapon2_A = 0;
  boolean depressed1  = false;
  boolean depressed2  = false;
  boolean validHit1  = false;
  boolean offTarget1 = false;
  boolean validHit2  = false;
  boolean offTarget2 = false;
};
struct State * state = (struct State *) malloc(sizeof(struct State));
struct WeaponState * weaponState = (struct WeaponState *) malloc(sizeof(struct WeaponState));
SoftwareSerial hc06(HC06_TX, HC06_RX); //Tx=2, Rx=3

void TimerHandler() {
  if (currentTime < 1){
    currentTime = 1; //just in case
  }
  if (state->inAction){
    currentTime -= 1;
    writeTime();
    Serial.println(currentTime);
    if (currentTime == 0){
      state->inAction = false;
    }
  }
}
void writeScore(int player, int score) {
  //write to either SCORE1 or SCORE2 with state->score1/2
}
void writeTime() {
  //Serial.println(currentTime);
  //write currentTime to the timer LEDs
  
  matrix.writeDigitRaw(2,0x02);

  int minute=currentTime/60;
  int seconds= (minute*60)-currentTime;
  int digitArray[2];
  String temp = String(seconds);
  digitArray[0]=temp[0];
  digitArray[1]=temp[1];
  
  matrix.writeDigitNum(1,minute);
  matrix.writeDigitNum(3,digitArray[0]);
  matrix.writeDigitNum(4,digitArray[1]);
  matrix.writeDisplay();
  
}



void setup() {
  //configure ADC
  DIDR0 = 0x7F;
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet  (ADCSRA, ADPS2);

  
  Serial.begin(9600);
  hc06.begin(9600);
  IrReceiver.begin(IR_RECV, ENABLE_LED_FEEDBACK);
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS, TimerHandler);

  
 
}
void loop() {
  while (1) {

    if (state->inAction){
      weaponState->weapon1_B = analogRead(WEAPON_1_PINB);
      weaponState->weapon2_B = analogRead(WEAPON_2_PINB);
      weaponState->weapon1_A = analogRead(WEAPON_1_PINA);
      weaponState->weapon2_B = analogRead(WEAPON_2_PINB);

      testBlades();
      signalHit();
    }
    
    //Check if there is any IR data to decode
    if (IrReceiver.decode()) {
      String cmd(IrReceiver.decodedIRData.decodedRawData, HEX);
      parseCommand(cmd);
      //Serial.println(cmd);
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
}

void testBlades() {

  long now = micros();

  //check if lockout time is here
  //A hit has occurred, and the lockout duration has passed.
  if (((weaponState->validHit1 || weaponState->offTarget1) && ((weaponState->depressTime1 + lockout[state->mode]) < now)) ||
      ((weaponState->validHit2 || weaponState->offTarget2) && (weaponState->depressTime2 + lockout[state->mode] < now))) {
    weaponState->lockedOut = true;
  }

  // weapon 1
  if (weaponState->validHit1 == false && weaponState->offTarget1 == false) { // ignore if 1 has already hit
    // off target
    if (offTargetVoltageB[state->mode]-89 < weaponState->weapon1_B && weaponState->weapon1_B < offTargetVoltageB[state->mode]+89 && 
        offTargetVoltageA[state->mode]-89 < weaponState->weapon2_A && weaponState->weapon2_A < offTargetVoltageA[state->mode]+89) { //weapon 1 and lame 2
      if (!weaponState->depressed1) { //if weapon1 just got depressed
        weaponState->depressTime1 = micros();
        weaponState->depressed1   = true;
      }
      else {
        if (weaponState->depressTime1 + depress[state->mode] <= micros()) { //if it has been depressed for enough time, set as off-target hit
          weaponState->offTarget1 = true;
        }
      }
    }
    else if (validVoltage[state->mode]-89 < weaponState->weapon2_B && weaponState->weapon2_B < validVoltage[state->mode]+89 && 
             validVoltage[state->mode]-89 < weaponState->weapon1_A && weaponState->weapon1_A < validVoltage+89) {
      // on target
      if (!weaponState->depressed1) {
        weaponState->depressTime1 = micros();
        weaponState->depressed1 = true;
      }
      else {
        if (weaponState->depressTime1 + depress[state->mode] <= micros()) {
          weaponState->validHit1 = true;
        }
      }
    }
    else {
      //weapon is no longer depressed, and depress time was too short. Reset
      weaponState->depressTime1 = 0;
      weaponState->depressed1 = false;
    }
  }
  // weapon 2
  if (weaponState->validHit2 == false && weaponState->offTarget2 == false) { // ignore if 2 has already hit
    // off target
    if (offTargetVoltageB[state->mode]-89 < weaponState->weapon2_B && weaponState->weapon2_B < offTargetVoltageB[state->mode]+89 && 
         offTargetVoltageA[state->mode]-89 < weaponState->weapon1_A && weaponState->weapon1_A < offTargetVoltageA[state->mode]+89){
     
      if (!weaponState->depressed2) { //if weapon 2 just got depressed
        weaponState->depressTime2 = micros();
        weaponState->depressed2   = true;
      }
      else {
        if (weaponState->depressTime2 + depress[state->mode] <= micros()) { //if it has been depressed for enough time, set as off-target hit
          weaponState->offTarget2 = true;
        }
      }
    }
    else if (validVoltage[state->mode]-89 < weaponState->weapon1_B && weaponState->weapon1_B < validVoltage[state->mode]+89 && 
             validVoltage[state->mode]-89 < weaponState->weapon2_A && weaponState->weapon2_A < validVoltage+89) {
      // on target
      if (!weaponState->depressed2) {
        weaponState->depressTime2 = micros();
        weaponState->depressed2 = true;
      }
      else {
        if (weaponState->depressTime2 + depress[state->mode] <= micros()) {
          weaponState->validHit2 = true;
        }
      }
    }
    else {
      //weapon is no longer depressed, and depress time was too short. Reset
      weaponState->depressTime2 = 0;
      weaponState->depressed2 = false;
    }
  }
}

//==============
void signalHit() {
   // non time critical, this is run after a hit has been detected
   if (weaponState->lockedOut) {
     Serial.println("signalHit");
     pause();
      //stop clock
      
      //sound buzzer
      
      if (weaponState->validHit1){
        state->score1 = state->score1 + 1;
        Serial.println("player 1 scored");
        //signal red light
      } 
      else if (weaponState->offTarget1){
        Serial.println("player 1 off-target");
        //signal left white
      }
      if (weaponState->validHit2){
        state->score2 = state->score2 + 1;
        Serial.println("player 2 scored");
        //signal green light
      }
      else if (weaponState->offTarget2){
        Serial.println("player 2 off target");
        //signal right white
      }
      resetValues();
   }
}

void resetValues() {
   delay(BUZZER_TIME);             // wait before turning off the buzzer
   //turn off buzzer
   delay(LIGHT_TIME);   // wait before turning off the lights
   //turn off all lights

   weaponState->lockedOut = false;
   weaponState->depressTime1 = 0;
   weaponState->depressed1 = false;
   weaponState->depressTime2 = 0;
   weaponState->depressed2  = false;

   weaponState->validHit1 = false;
   weaponState->offTarget1 = false;
   weaponState->validHit2  = false;
   weaponState->offTarget2 = false;
}
void pause(){
    state->inAction = false;
    if (currentTime > 99){
      char buffer [3];
      char msg [10] = {'p','a','u','s','e','0','0','0','\n','\0'};
      
     
      itoa(currentTime, buffer, 10);
      for (int i = 5; i<8; i++){
        msg[i] = buffer[i-5];
      }
     // Serial.println(msg);
      
      hc06.write(msg);
    }
    else if (currentTime > 9) {
      char buffer [2];
      char msg [] = {'p','a','u','s','e','0','0','\n','\0'};
      itoa(currentTime, buffer, 10);
      for (int i = 5; i<7; i++){
        msg[i] = buffer[i-5];
      }
      //Serial.println(msg);
      
      hc06.write(msg);
    }
    else  {
      char buffer [1];
      char msg [] = {'p','a','u','s','e','0','\n','\0'};
      itoa(currentTime, buffer, 10);
      msg[5] = buffer[0];
      
      //Serial.println(msg);
    
      hc06.write(msg);
    }
}
void parseCommand(String cmd) {
  if (state->inAction == false && (cmd == "play\n" || cmd == "bb44ff00") && (currentTime > 0)){
    state->inAction = true;
    if (cmd = "bb44ff00"){
      hc06.write("play\n");
    }
    Serial.println("play");
  }
  else if (state->inAction == true && (cmd == "pause\n" || cmd == "bb44ff00")){
    pause();
    
  }
  if (cmd == "inc1\n" || cmd == "f708ff00") {
    state->score1 += 1;
    //write new score to SCORE1
    Serial.println("inc1");
    if (cmd == "f708ff00") {
      hc06.write("inc1\n");
    }
  }
  else if (cmd == "inc2\n" || cmd == "a55aff00") {
    state->score2 += 1;
    //write new score to SCORE2
    Serial.println("inc2");
    if (cmd == "a55aff00") {
      hc06.write("inc2\n");
    }
  }
  else if (cmd == "dec1\n" || cmd == "bd42ff00") {
    if (state->score1 > 0) {
      state->score1 -= 1;
      //write new score to SCORE1
    }
    Serial.println("dec1");
    if (cmd == "bd42ff00") {
      hc06.write("dec1\n");
    }
  }
  else if (cmd == "dec2\n" || cmd == "b54aff00") {
    if (state->score2 > 0) {
      state->score2 -= 1;
      //write new score to SCORE2
    }
    Serial.println("dec2");
    if (cmd == "b54aff00") {
      hc06.write("dec2\n");
    }
  }
  else if (cmd == "mute\n" || cmd == "b847ff00") {
    if (state->muted == 1) {
      state->muted == 0;
    }
    else {
      state->muted == 1;
    }
    Serial.println("mute");
  }

  else if (cmd == "timer1\n" || cmd == "f30cff00") {
    currentTime = 60;
    state->inAction = true;
    Serial.println("timer1");
    if (cmd == "f30cff00") {
      hc06.write("timer1\n");
    }
  }
  else if (cmd == "timer3\n" || cmd == "a15eff00") {
    currentTime = 180;
    state->inAction = true;
    Serial.println("timer3");
    if (cmd == "a15eff00") {
      hc06.write("timer3\n");
    }
  }
  else if (cmd == "foil\n") {
    state->mode = FOIL;
    Serial.println("foil");
  }
  else if (cmd == "epee\n") {
    state->mode = EPEE;
    Serial.println("epee");
  }
  else if (cmd == "sabre\n") {
    state->mode = SABRE;
    Serial.println("sabre");
  }
  else if (cmd == "mode\n" || cmd == "b946ff00") {
    Serial.println("mode");
    if (state->mode == FOIL) {
      state->mode = EPEE;
    }
    else if (state->mode == EPEE) {
      state->mode = SABRE;
    }
    else {
      state->mode = FOIL;
    }
    if (cmd == "b946ff00") {
      hc06.write("mode\n");
    }
  }
}
