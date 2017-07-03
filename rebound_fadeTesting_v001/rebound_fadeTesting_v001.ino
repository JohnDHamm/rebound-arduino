
/*
  rebound - test for fading non-PWM

*/

#include <ShiftRegister.h>

ShiftRegister sr(1, 8, 10, 9, 12);


int j=0;    //for ballPos array
int b=0;  //for brightness array
int brightnessPin = 3; //PWM pin connected to SR pin 13 OE

int fade1=50;
int fade2=5;
  
int numLEDs[]={1,2,3,4,4,4,4,4};
int brightness[]={255,fade1,fade2,0};
int ballPos[]={0,1,2,3,4,5,6,7};

int ballSpeed=1000;    //initial time between ball positions

//------------------------------setup---------------------------------

void setup() {

  pinMode(brightnessPin, OUTPUT); 

Serial.begin(9600);
  sr.setShiftOrder(MSBFIRST);  //set order in which bits are shifted (MSBFIRST/LSBFIRST)
  sr.clear();                  //set all bits to low
  
}

void loop() {
  delay(1000);
  runUp();
  

}




//------------------functions---------------------------
void runUp() {

  for (int m=0; m<8; m++); {
    j=m;
    b=0;
    for (int i=0; i<numLEDs[m]; i++) {
      allOff();
      analogWrite(brightnessPin, brightness[b]);
      sr.setBit(ballPos[j], HIGH);
      //checkButton();
      j--;
      b++;
      delay(50);    //
    }
  }
}


void allOff() {
  for (int i=0; i<8; i++) {
    sr.setBit(i, LOW);
  }
}


