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
#include <avr/wdt.h>
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#define USE_TIMER_1 true
#define DECODE_NEC
#include <IRremote.hpp>
#include <SoftwareSerial.h>
#include "TimerInterrupt.h"
#define TIMER_INTERVAL_MS 1000


#define BUZZER 0
#define HC06_RX 3
#define HC06_TX 2
#define IR_RECV 4

#define DATA 5
#define SRCLK1 6
#define RCLK1 7
#define SRCLK2 8
#define RCLK2 9

#define WHITE_1 10 //red is player 2
#define GREEN 11 //green is player 1
#define RED 12
#define WHITE_2 1



#define WEAPON_1_PINA A0 // Lame   A pin - Analog (Epee return path)
#define WEAPON_1_PINB A1 // Weapon A pin - Analog
#define WEAPON_2_PINA A2 // Lame   B pin - Analog (Epee return path)
#define WEAPON_2_PINB A3 // Weapon B pin - Analog

#define TIMER_SCL A5
#define TIMER_SDA A4

#define FOIL 0
#define EPEE 1
#define SABRE 2

Adafruit_7segment matrix = Adafruit_7segment();

// depress time is the amount of time needed to maintain valid contact for a hit to count
const unsigned long depress[] = {14000, 2000, 1000}; // foil, epee, sabre
// lockout is the amount of time after a hit to allow the other person to get a hit, before the round ends
const unsigned long lockout[] = {300000, 45000, 120000};
const int validVoltageLow[] = {420, 420, 240}; // its the same voltage for pin A and B on a valid hit
const int validVoltageHigh[] = {600, 600, 420};

const int offTargetVoltageLowA[] = { -1, 240, -2}; // sabre doesn't have off target
const int offTargetVoltageHighA[] = {100, 420, -2};
const int offTargetVoltageLowB[] = {850, 240, -2};
const int offTargetVoltageHighB[] = {1025, 420, -2};

#define BUZZER_TIME 2000 // buzzer for 2 seconds
#define LIGHT_TIME 2000  // lights stay on for 2 additional seconds

int j = 0;
int currentTime = 0;
int write_time = false;
struct State
{
  bool inAction = false;
  int score1 = 0;
  int score2 = 0;
  int mode = EPEE;
  bool muted = false;
};

struct WeaponState
{
  unsigned long depressTime1 = 0;
  unsigned long depressTime2 = 0;
  bool lockedOut = false;
  int weapon1_B = 0;
  int weapon2_B = 0;
  int weapon1_A = 0;
  int weapon2_A = 0;
  bool depressed1 = false;
  bool depressed2 = false;
  bool validHit1 = false;
  bool offTarget1 = false;
  bool validHit2 = false;
  bool offTarget2 = false;
};
struct State *state = (struct State *)malloc(sizeof(struct State));
struct WeaponState *weaponState = (struct WeaponState *)malloc(sizeof(struct WeaponState));


SoftwareSerial hc06(HC06_TX, HC06_RX); // Tx=2, Rx=3

void TimerHandler()
{
  if (state->inAction == true)
  {
    if (currentTime < 1 || currentTime > 180)
    {
      currentTime = 1; // just in case
    }
    currentTime -= 1;
    write_time = true;
    //Serial.println(currentTime);

    if (currentTime == 0)
    {
      state->inAction = false;
    }
  }
}

void setup()
{
  matrix.begin(0x70);
  // configure ADC
  DIDR0 = 0x0F; //disabling digital inputs for A0-A3
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2); // /16 prescaler

  pinMode(DATA, OUTPUT);
  pinMode(SRCLK1, OUTPUT);
  pinMode(RCLK1, OUTPUT);
  pinMode(SRCLK2, OUTPUT);
  pinMode(RCLK2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(WHITE_1, OUTPUT);
  pinMode(WHITE_2, OUTPUT);
  digitalWrite(GREEN, 0);
  digitalWrite(RED, 0);
  digitalWrite(WHITE_1, 0);
  digitalWrite(WHITE_2, 0);

  //Instantiate the struct variables. (they don't instantiate within the definition)
  state->inAction = false;
  state->score1 = 0;
  state->score2 = 0;
  state->mode = EPEE;
  state->muted = false;
  weaponState->depressTime1 = 0;
  weaponState->depressTime2 = 0;
  weaponState->lockedOut = false;
  weaponState->weapon1_B = 0;
  weaponState->weapon1_A = 0;
  weaponState->weapon2_B = 0;
  weaponState->weapon2_A = 0;
  weaponState->depressed1 = false;
  weaponState->depressed2 = false;
  weaponState->validHit1 = false;
  weaponState->offTarget1 = false;
  weaponState->validHit2 = false;
  weaponState->offTarget2 = false;

  ////Serial.begin(9600);
  hc06.begin(9600);
  IrReceiver.begin(IR_RECV, ENABLE_LED_FEEDBACK);
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS, TimerHandler);
 
  matrix.begin(0x70);
  matrix.writeDigitRaw(2, 0x02);
  matrix.writeDigitNum(0, 0);
  matrix.writeDigitNum(1, 0);
  matrix.writeDigitNum(3, 0);
  matrix.writeDigitNum(4, 0);
  matrix.writeDisplay();

  resetMatch();

  wdt_enable(WDTO_8S);//watchdog timer. This will reset the program if it gets frozen
}
void loop()
{
  while (1)
  {
    if (write_time == true) {

      writeTime();
      write_time = false;
      //Serial.println("write");
    }
    if (state->inAction == true)
    {
      if (weaponState->lockedOut == false){
        weaponState->weapon1_B = analogRead(WEAPON_1_PINB);
        weaponState->weapon2_B = analogRead(WEAPON_2_PINB);
        weaponState->weapon1_A = analogRead(WEAPON_1_PINA);
        weaponState->weapon2_A = analogRead(WEAPON_2_PINA);

        testBlades();
      }
      if (weaponState->lockedOut == true){      
        signalHit();
      }

    }

    // Check if there is any IR data to decode
    if (IrReceiver.decode())
    {
      decodeIR();
    }

    // Check if there is any bluetooth data to decode
    if (hc06.available()) {
      decodeBluetooth();
    }
    wdt_reset();//reset watchdog timer.
  }
}
void decodeIR()
{
  uint32_t rawCmd = IrReceiver.decodedIRData.decodedRawData;
  uint32_t mask = 0b00000000000000000000000000001111;
  char cmd[9] = {'0','0','0','0','0','0','0','0','\0'};
  for (int i = 7; i>=0; i++){
    uint32_t digit = mask & rawCmd;
    char thisChar = (char) digit;
    //convert hex to ascii equivalent
    if (thisChar > 9){
      thisChar += 87; 
    }
    else{
      thisChar += 48;
    }
    
    cmd[i] = thisChar;
    rawCmd = rawCmd >> 4;
  }
 
  parseCommand(cmd);
  IrReceiver.resume();
}
void decodeBluetooth()
{
  char buffer[10];
  int index = 0;
  while (index < 10 && index > -1)
  {
    if (hc06.available())
    {
      buffer[index] = hc06.read();
      if (buffer[index] == '\n')
      { // end of message
        char cmd[index + 2];
        for (int i = 0; i <= index; i++)
        {
          cmd[i] = buffer[i];
        }
        cmd[index + 1] = '\0'; // cmd is a char array with a \n and \0 as the last 2 chars, to make it a string
        parseCommand(cmd);
        index = 10; // exit
      }
      else
      {
        index++;
      }
    }
  }
}

void writeTime()
{
  // //Serial.println(currentTime);
  // write currentTime to the timer LEDs

  matrix.writeDigitRaw(2, 0x02);

  int minute = currentTime / 60;
  int seconds =  currentTime - (minute * 60);
  int digitArray[2];
  digitArray[0] = seconds / 10;
  digitArray[1] = seconds - (seconds / 10) * 10;

  matrix.writeDigitNum(0, 0);
  matrix.writeDigitNum(1, minute);
  matrix.writeDigitNum(3, digitArray[0]);
  matrix.writeDigitNum(4, digitArray[1]);
  matrix.writeDisplay();
}

void testBlades()
{

  unsigned long now = micros();

  int *weapon1, *weapon2, *target1, *target2;
  if (state->mode == EPEE)
  {
    weapon1 = &weaponState->weapon1_B;
    target1 = &weaponState->weapon1_A; // player 1's blade must connect to their own A line

    weapon2 = &weaponState->weapon2_B;
    target2 = &weaponState->weapon2_A;
  }
  else
  { // FOIL and SABRE
    weapon1 = &weaponState->weapon1_B;
    target1 = &weaponState->weapon2_A; // player 1's blade must hit player 2's lame (A line)

    weapon2 = &weaponState->weapon2_B;
    target2 = &weaponState->weapon1_A;
  }
#ifdef DEBUG_VOLTAGE
  if (j % 10000000 == 0) {
    //Serial.print("1A:");
    //Serial.println(weaponState->weapon1_A);
    //Serial.print("1B:");
    //Serial.println(weaponState->weapon1_B);
    //Serial.print("2A:");
    //Serial.println(weaponState->weapon2_A);
    //Serial.print("2B:");
    //Serial.println(weaponState->weapon2_B);
  }
  if (j > 100000000) {
    j = 0;
  }
#endif

  // check if lockout time is here
  // A hit has occurred, and the lockout duration has passed.
  if (((weaponState->validHit1 || weaponState->offTarget1) && ((weaponState->depressTime1 + lockout[state->mode]) < now)) ||
      ((weaponState->validHit2 || weaponState->offTarget2) && (weaponState->depressTime2 + lockout[state->mode] < now)))
  {
    weaponState->lockedOut = true;
  }

  // weapon 1
  if (weaponState->validHit1 == false && weaponState->offTarget1 == false)
  { // ignore if 1 has already hit
    // off target
    if ((offTargetVoltageLowB[state->mode] < *weapon1) &&
        (*weapon1 < offTargetVoltageHighB[state->mode]) &&
        (offTargetVoltageLowA[state->mode] < *target1) &&
        (*target1 < offTargetVoltageHighA[state->mode]))
    {

      if (weaponState->depressed1 == false)
      { // if weapon1 just got depressed
        weaponState->depressTime1 = micros();
        weaponState->depressed1 = true;
      }
      else
      {
        if (weaponState->depressTime1 + depress[state->mode] <= micros())
        { // if it has been depressed for enough time, set as off-target hit
          weaponState->offTarget1 = true;
        }
      }
    } // on target
    else if ((validVoltageLow[state->mode] < *weapon1) &&
             (*weapon1 < validVoltageHigh[state->mode]) &&
             (validVoltageLow[state->mode] < *target1) &&
             (*target1 < validVoltageHigh[state->mode]))
    {

      if (weaponState->depressed1 == false)
      {
        weaponState->depressTime1 = micros();
        weaponState->depressed1 = true;
      }
      else
      {
        if (weaponState->depressTime1 + depress[state->mode] <= micros())
        {
          weaponState->validHit1 = true;
        }
      }
    }
    else
    {
      // weapon is no longer depressed, and depress time was too short. Reset
      weaponState->depressTime1 = micros() + 99999999;
      weaponState->depressed1 = false;
    }
  }
  // weapon 2
  if (weaponState->validHit2 == false && weaponState->offTarget2 == false)
  { // ignore if 2 has already hit
    // off target
    if ((offTargetVoltageLowB[state->mode] < *weapon2) &&
        (*weapon2 < offTargetVoltageHighB[state->mode]) &&
        (offTargetVoltageLowA[state->mode] < *target2) &&
        (*target2 < offTargetVoltageHighA[state->mode]))
    {

      if (weaponState->depressed2 == false)
      { // if weapon 2 just got depressed
        weaponState->depressTime2 = micros();
        weaponState->depressed2 = true;
      }
      else
      {
        if (weaponState->depressTime2 + depress[state->mode] <= micros())
        { // if it has been depressed for enough time, set as off-target hit
          weaponState->offTarget2 = true;
        }
      }
    } // on target
    else if ((validVoltageLow[state->mode] < *weapon2) &&
             (*weapon2 < validVoltageHigh[state->mode]) &&
             (validVoltageLow[state->mode] < *target2) &&
             (*target2 < validVoltageHigh[state->mode]))
    {

      if (weaponState->depressed2 == false)
      {
        weaponState->depressTime2 = micros();
        weaponState->depressed2 = true;
      }
      else
      {
        if (weaponState->depressTime2 + depress[state->mode] <= micros())
        {
          weaponState->validHit2 = true;
        }
      }
    }
    else
    {
      // weapon is no longer depressed, and depress time was too short. Reset
      weaponState->depressTime2 = micros() + 9999999;
      weaponState->depressed2 = false;
    }
  }
}

// this is run after a hit has been detected
void signalHit()
{

  //Serial.println("signalHit");
  pause(); // sets inAction to false and sends pause command via bluetooth

  // sound buzzer
#ifdef DEBUG_VOLTAGE
  //Serial.print("1A:");
  //Serial.println(weaponState->weapon1_A);
  //Serial.print("1B:");
  //Serial.println(weaponState->weapon1_B);
  //Serial.print("2A:");
  //Serial.println(weaponState->weapon2_A);
  //Serial.print("2B:");
  //Serial.println(weaponState->weapon2_B);
  //Serial.println(offTargetVoltageLowA[state->mode]);
  //Serial.println(offTargetVoltageHighA[state->mode]);
  //Serial.println(offTargetVoltageLowB[state->mode]);
  //Serial.println(offTargetVoltageHighB[state->mode]);
  //Serial.println(state->mode);
#endif
  if (state->muted == false){
    digitalWrite(BUZZER, 1);
  }
  if (weaponState->validHit1 == true)
  {
    if (state->mode == EPEE){
      state->score1 += 1;
      hc06.write("inc1\n");
      writeScore(state->score1, 1);
    }
    
    //Serial.println("player 1 scored");
    digitalWrite(GREEN, 1);
  }
  else if (weaponState->offTarget1 == true)
  {
    //Serial.println("player 1 off-target");
    digitalWrite(WHITE_1, 1);
  }
  if (weaponState->validHit2 == true)
  {
    if (state->mode == EPEE){
      state->score2 += 1;
      hc06.write("inc2\n");
      writeScore(state->score2, 2);
    }
   
    //Serial.println("player 2 scored");
    digitalWrite(RED, 1);
  }
  else if (weaponState->offTarget2 == true)
  {
    //Serial.println("player 2 off target");
    digitalWrite(WHITE_2, 1);
  }

  delay(BUZZER_TIME); // wait before turning off the buzzer
  digitalWrite(BUZZER, 0);
  delay(LIGHT_TIME); // wait before turning off the lights
  digitalWrite(RED, 0);
  digitalWrite(GREEN, 0);
  digitalWrite(WHITE_1, 0);
  digitalWrite(WHITE_2, 0);
  resetValues();
}

void resetValues()
{
  //Serial.println("reset");

  state->inAction = false;
  weaponState->lockedOut = false;

  unsigned long d = micros() + 9999999;

  weaponState->depressTime1 = d;
  weaponState->depressed1 = false;
  weaponState->depressTime2 = d;
  weaponState->depressed2 = false;

  weaponState->validHit1 = false;
  weaponState->offTarget1 = false;
  weaponState->validHit2 = false;
  weaponState->offTarget2 = false;

  digitalWrite(GREEN, 0);
  digitalWrite(RED, 0);
  digitalWrite(WHITE_1, 0);
  digitalWrite(WHITE_2, 0);
  
}
void resetMatch(){
  resetValues();
  currentTime = 0;
  state->score1 = 0;
  state->score2 = 0;
  writeScore(0,1);
  writeScore(0,2);

  matrix.writeDigitNum(0, 0);
  matrix.writeDigitNum(1, 0);
  matrix.writeDigitNum(3, 0);
  matrix.writeDigitNum(4, 0);
  matrix.writeDisplay();
}
void pause()
{
  //Serial.println("pause");
  state->inAction = false;
  if (currentTime > 99)
  {
    char buffer[4] = {'0','0','0','\0'};
    char msg[10] = {'p', 'a', 'u', 's', 'e', '0', '0', '0', '\n', '\0'};

    itoa(currentTime, buffer, 10);
    for (int i = 5; i < 8; i++)
    {
      msg[i] = buffer[i - 5];
    }

    hc06.write(msg);
  }
  else if (currentTime > 9)
  {
    char buffer[3] = {'0','0','\0'};
    char msg[9] = {'p', 'a', 'u', 's', 'e', '0', '0', '\n', '\0'};
    itoa(currentTime, buffer, 10);
    for (int i = 5; i < 7; i++)
    {
      msg[i] = buffer[i - 5];
    }

    hc06.write(msg);
  }
  else
  {
    char buffer[2] = {'0','\0'};
    char msg[8] = {'p', 'a', 'u', 's', 'e', '0', '\n', '\0'};
    itoa(currentTime, buffer, 10);
    msg[5] = buffer[0];

    hc06.write(msg);
  }
}
void parseCommand(char cmd[])
{
  if (cmd == "reset\n" || cmd == "e619ff00") {
    state->inAction = false;
    resetMatch();
    if (cmd == "e619ff00") {
      hc06.write("reset\n");
    }
    //Serial.println("reset");
  }
  else if (state->inAction == false && (cmd == "play\n" || cmd == "bb44ff00") && (currentTime > 0))
  {
    state->inAction = true;
    if (cmd == "bb44ff00")
    {
      hc06.write("play\n");
    }
    //Serial.println("play");
  }
  else if (state->inAction == true && (cmd == "pause\n" || cmd == "bb44ff00"))
  {
    pause();
  }
  if (cmd == "inc1\n" || cmd == "f708ff00")
  {
    state->score1 += 1;
    // write new score to SCORE1
    //Serial.println("inc1");
    writeScore(state->score1, 1);
    if (cmd == "f708ff00")
    {
      hc06.write("inc1\n");
    }
  }
  else if (cmd == "inc2\n" || cmd == "a55aff00")
  {
    state->score2 += 1;
    // write new score to SCORE2
    //Serial.println("inc2");
    writeScore(state->score2, 2);
    if (cmd == "a55aff00")
    {
      hc06.write("inc2\n");
    }
  }
  else if (cmd == "dec1\n" || cmd == "bd42ff00")
  {
    if (state->score1 > 0)
    {
      state->score1 -= 1;
      // write new score to SCORE1
    }
    //Serial.println("dec1");
    writeScore(state->score1, 1);
    if (cmd == "bd42ff00")
    {
      hc06.write("dec1\n");
    }
  }
  else if (cmd == "dec2\n" || cmd == "b54aff00")
  {
    if (state->score2 > 0)
    {
      state->score2 -= 1;
      // write new score to SCORE2
    }
    //Serial.println("dec2");
    writeScore(state->score2, 2);
    if (cmd == "b54aff00")
    {
      hc06.write("dec2\n");
    }
  }
  else if (cmd == "mute\n" || cmd == "b847ff00")
  {
    if (state->muted == true)
    {
      state->muted = false;
      matrix.writeDigitAscii(0,'_');
      matrix.writeDigitAscii(1,'_');
      matrix.writeDigitAscii(3,'O');
      matrix.writeDigitAscii(4,'n');
      matrix.writeDisplay();
      
    }
    else
    {
      state->muted = true;
      matrix.writeDigitAscii(0,'_');
      matrix.writeDigitAscii(1,'O');
      matrix.writeDigitAscii(3,'F');
      matrix.writeDigitAscii(4,'F');
      matrix.writeDisplay();
    }
    //Serial.println("mute");
  }

  else if (cmd == "timer1\n" || cmd == "f30cff00")
  {
    currentTime = 60;
    state->inAction = true;
    //Serial.println("timer1");
    if (cmd == "f30cff00")
    {
      hc06.write("timer1\n");
    }
  }
  else if (cmd == "timer3\n" || cmd == "a15eff00")
  {
    currentTime = 180;
    state->inAction = true;
    //Serial.println("timer3");
    if (cmd == "a15eff00")
    {
      hc06.write("timer3\n");
    }
  }
  else if (cmd == "foil\n")
  {
    state->mode = EPEE;
    resetMatch();
    matrix.writeDigitAscii(0,'E');
    matrix.writeDigitAscii(1,'P');
    matrix.writeDigitAscii(3,'E');
    matrix.writeDigitAscii(4,'E');
    matrix.writeDisplay();
    write_time = false;
    ////Serial.println("foil");
  }
  else if (cmd == "epee\n")
  {
    state->mode = SABRE;
    resetMatch();
    matrix.writeDigitAscii(0,'S');
    matrix.writeDigitAscii(1,'A');
    matrix.writeDigitAscii(3,'b');
    matrix.writeDigitAscii(4,'r');
    matrix.writeDisplay();
    write_time = false;
    ////Serial.println("epee");
  }
  else if (cmd == "sabre\n")
  {
    state->mode = FOIL;
    resetMatch();
    matrix.writeDigitAscii(0,'F');
    matrix.writeDigitAscii(1,'O');
    matrix.writeDigitAscii(3,'I');
    matrix.writeDigitAscii(4,'L');
    matrix.writeDisplay();
    write_time = false;
    ////Serial.println("sabre");
  }
  else if (cmd == "mode\n" || cmd == "b946ff00")
  {
    ////Serial.println("mode");
    if (state->mode == FOIL)
    {
      state->mode = EPEE;
      resetMatch();
      matrix.writeDigitAscii(0,'E');
      matrix.writeDigitAscii(1,'P');
      matrix.writeDigitAscii(3,'E');
      matrix.writeDigitAscii(4,'E');
      matrix.writeDisplay();
      hc06.write("epee\n");
      write_time = false;
    }
    else if (state->mode == EPEE)
    {
      state->mode = SABRE;
      resetMatch();
      matrix.writeDigitAscii(0,'S');
      matrix.writeDigitAscii(1,'A');
      matrix.writeDigitAscii(3,'b');
      matrix.writeDigitAscii(4,'r');
      matrix.writeDisplay();      
      hc06.write("sabre\n");
      write_time = false;
    }
    else if (state->mode == SABRE)
    {
      state->mode = FOIL;
      resetMatch();
      matrix.writeDigitAscii(0,'F');
      matrix.writeDigitAscii(1,'O');
      matrix.writeDigitAscii(3,'I');
      matrix.writeDigitAscii(4,'L');
      matrix.writeDisplay();  
      hc06.write("foil\n");  
      write_time = false;  
    }
    else
    {
      hc06.write("mode\n");
    }
  }
  else {
    //Serial.println(cmd);
  }
}
/*
  digit is 0-19
  display is 1 or 2
*/
void writeScore(int digit, int displayNum) {
  //Serial.println(digit);
  int srclk = SRCLK1;
  int rclk = RCLK1;
  if (displayNum == 2) {
    srclk = SRCLK2;
    rclk = RCLK2;
  }
  if (digit > 19) {
    digit = 19;
  }
  uint8_t output = 0b00000000; //8 bits to be shifted. The first bit is the 1 on the first display
  switch (digit) {
    case 0:
      output = 0b00111111;
      break;
    case 10:
      output = 0b10111111;
      break;
    case 1:
      output = 0b00001001;
      break;
    case 11:
      output = 0b10001001;
      break;
    case 2:
      output = 0b01011110;
      break;
    case 12:
      output = 0b11011110;
      break;
    case 3:
      output = 0b01011011;
      break;
    case 13:
      output = 0b11011011;
      break;
    case 4:
      output = 0b01101001;
      break;
    case 14:
      output = 0b11101001;
      break;
    case 5:
      output = 0b01110011;
      break;
    case 15:
      output = 0b11110011;
      break;
    case 6:
      output = 0b01110111;
      break;
    case 16:
      output = 0b11110111;
      break;
    case 7:
      output = 0b00011001;
      break;
    case 17:
      output = 0b10011001;
      break;
    case 8:
      output = 0b01111111;
      break;
    case 18:
      output = 0b11111111;
      break;
    case 9:
      output = 0b01111011;
      break;
    case 19:
      output = 0b11111011;
      break;

    default:
      break;
  }
  for (int i = 0; i < 8; i++) {
    uint8_t data = 0b00000001 & output; //isolate the rightmost bit
    //Serial.println(data);
    digitalWrite(DATA, data);
    delayMicroseconds(1);
    digitalWrite(srclk, 1);
    delayMicroseconds(1);
    digitalWrite(srclk, 0);
    output = output >> 1;
    delayMicroseconds(1);
  }
  digitalWrite(rclk, 1);
  delayMicroseconds(1);
  digitalWrite(rclk, 0);

}
