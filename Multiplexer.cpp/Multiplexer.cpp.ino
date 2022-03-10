#include <bitset>

#include namespace std

const int sel_0 = 0;
const int sel_1 = 8;     //initializing pin 8 as sel_1 variable
const int sel_2 = 0;
const int sel_3 = 0;
//const int sel_4 = 0;
//const int sel_5 = 0;
//const int sel_6 = 0;
//const int sel_7 = 0;`
const int En_1 = 0;
const int En_2 = 0;
const int data = 0;       //data on this pin

const int[7] pins = {sel_0, sel_1, sel_2, sel_3, En_1, En_2, data};

/** Function Prototype */
void assignData();
void clearData();
void outputDigit(int digit, bool Left);

void setup()
{
  pinMode(sel_0,OUTPUT);   //Set pin 2 as output
  pinMode(sel_1,OUTPUT);
  pinMode(sel_2,OUTPUT);
  pinMode(sel_3,OUTPUT);
  pinMode(En_1,OUTPUT);
  pinMode(En_2,OUTPUT);
  pinMode(data,OUTPUT);
}

void loop()
{
  while(true)
  {
    delay(1000);
    TEST();
  }
  
}

/**
 * Assign data to pin
 */
void assignData(int select,int En_1,int En_2,int output)
{
  string binary = bitset<4>(n).to_string(select); // 1 <= select <= 16
  digitalWrite(pins[0], binary.at(0));
  digitalWrite(pins[1], binary.at(1));
  digitalWrite(pins[2], binary.at(2));
  digitalWrite(pins[3], binary.at(3));
  digitalWrite(pins[4], En_1);   // enable_1
  digitalWrite(pins[5], En_2);   // enable_2
  digitalWrite(pins[6], output);   // data
}

/**
 * Clear data on all pins
 */
void clearData()
{
  for (int i = 1, i < 15, i++)
  {
    assignData(i,1,0,0);
    assignData(i,0,1,0);
  }
}

/**
 * Display Number
 */
void TEST()
{
  outputDigit(4,1);
  outputDigit(5,2);
}


void outputDigit(int digit, int numDisplay)
{
  if (digit == 0){                // 0
    if (numDisplay == 1){
      assignData(2,1,0,1);
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(5,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){  
      assignData(9,1,0,1);
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(12,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){  
      assignData(2,0,1,1);
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(5,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){  
      assignData(9,0,1,1);
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(12,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 1){
    if (numDisplay == 1){
      assignData(4,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(11,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(4,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(11,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 2){         // 2
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(5,1,0,1);
      assignData(6,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(12,1,0,1);
      assignData(13,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(5,0,1,1);
      assignData(6,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(12,0,1,1);
      assignData(13,0,1,1);
    }
  } else if (digit == 3){         // 3
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 4){         // 4
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(2,1,0,1);
      assignData(4,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(9,1,0,1);
      assignData(11,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(2,0,1,1);
      assignData(4,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(9,0,1,1);
      assignData(11,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 5){         // 5
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(2,1,0,1);
      assignData(3,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(9,1,0,1);
      assignData(10,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(2,0,1,1);
      assignData(3,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(9,0,1,1);
      assignData(10,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 6){         // 6
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(2,1,0,1);
      assignData(3,1,0,1);
      assignData(5,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(9,1,0,1);
      assignData(10,1,0,1);
      assignData(12,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(2,0,1,1);
      assignData(3,0,1,1);
      assignData(5,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(9,0,1,1);
      assignData(10,0,1,1);
      assignData(12,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 7){
    if (numDisplay == 1){
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(6,1,0,1);
    } else if (numDisplay == 2){
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(13,1,0,1);
    } else if (numDisplay == 3){
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(6,0,1,1);
    } else if (numDisplay == 4){
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(13,0,1,1);
    }
  } else if (digit == 8){
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(2,1,0,1);
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(5,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(9,1,0,1);
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(12,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(2,0,1,1);
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(5,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(9,0,1,1);
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(12,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  } else if (digit == 9){
    if (numDisplay == 1){
      assignData(1,1,0,1);
      assignData(2,1,0,1);
      assignData(3,1,0,1);
      assignData(4,1,0,1);
      assignData(6,1,0,1);
      assignData(7,1,0,1);
    } else if (numDisplay == 2){
      assignData(8,1,0,1);
      assignData(9,1,0,1);
      assignData(10,1,0,1);
      assignData(11,1,0,1);
      assignData(13,1,0,1);
      assignData(14,1,0,1);
    } else if (numDisplay == 3){
      assignData(1,0,1,1);
      assignData(2,0,1,1);
      assignData(3,0,1,1);
      assignData(4,0,1,1);
      assignData(6,0,1,1);
      assignData(7,0,1,1);
    } else if (numDisplay == 4){
      assignData(8,0,1,1);
      assignData(9,0,1,1);
      assignData(10,0,1,1);
      assignData(11,0,1,1);
      assignData(13,0,1,1);
      assignData(14,0,1,1);
    }
  }
}
