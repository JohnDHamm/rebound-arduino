
/*
rebound build 06 - add RGB LED with result feedback
  + correct button being pressed for period of time (ignore after 1st press)
  + start sequence
  
  build 05 - 1 player game setup (no brightness controls of LED runs)

*/

#include <ShiftRegister.h>

ShiftRegister sr(1, 8, 10, 9, 12);

int buttonApin = 13;

int ballSpeed = 300;    //initial time between ball positions
int speedIncrease = 0;    //factor to increase speed

//int turn=1;          //keep track of how many "turns" (runs)
//int score=0;          //scoring value

int buttonPress = 0;    //track if button pushed in time 0=no, 1=yes

int earlyZone = 50;    //% of ballSpeed where button press in rebound zone is too early
int goodZone = 50;    //% of ballSpeed where button press in rebound zone is good (early + good =100=interval between led 0 and home)
int lateZone = 50;    //% of ballSpeed where button press in rebound zone is late, but accepted

int redHomePin = 3;      //pin for homeRGB LED connected to red
int greenHomePin = 5;    //pin for homeRGB LED connected to red
int blueHomePin = 6;      //pin for homeRGB LED connected to red


//------------------------------setup---------------------------------

void setup() {

  pinMode(buttonApin, INPUT_PULLUP);

Serial.begin(9600);
  sr.setShiftOrder(MSBFIRST);  //set order in which bits are shifted (MSBFIRST/LSBFIRST)
  sr.clear();                  //set all bits to low


//start sequence - home LED blinks purple 3x, then green once
for (int i = 0; i < 3; i++) {
  allOff();
  analogWrite(redHomePin, 0);
  analogWrite(blueHomePin, 0);
  delay (50);
  allOff();
  delay (ballSpeed - 50);
}

analogWrite(greenHomePin, 0);
delay (50);
allOff();
delay (ballSpeed - 50);

}
  
//-----------------------------game loop---------------------------------------

void loop() {

  runUpDown();    
  reboundCheck();
  if (buttonPress == 0) {
    endGameLate();
  }
  if (buttonPress == 1) {
    ballSpeedIncrease();
  }
}


//------------------functions---------------------------

//cycle of LEDs from start to end (0-7) and back (6-1) with check for early button push
void runUpDown() {     
   buttonPress=0;
   for (int i = 0; i < 8; i++) {  
     for (int k = 0; k < 100; k++) {
       sr.setBit(i, HIGH);
       checkButtonBad();      
       delay(ballSpeed / 100);
     }       
    sr.setBit(i, LOW);              
  }
  allOff();    // to turn off home RGB LED feedback from previous rebound
  for (int i = 6; i > 0; i--) {
    for (int k = 0; k < 100; k++) {
      sr.setBit(i, HIGH); 
      checkButtonBad();
      delay(ballSpeed / 100);
    }      
    sr.setBit(i, LOW);
  }
}


//check bad button press during runUpDown + early rebound
void checkButtonBad() {          
    if (digitalRead(buttonApin) == LOW) {
      endGameEarly();
    }
}


//check for button press in the rebound "zone"
void reboundCheck() {
  for (int k = 0; k < earlyZone; k++) {    //check for early button press 1st 50% of interval time
       sr.setBit(0, HIGH);
       checkButtonBad();      
       delay(ballSpeed / 100);
     } 
  for (int k = 0; k < goodZone; k++) {    //check for early button press last 50% of interval time
       checkButtonGood();      
       delay(ballSpeed / 100);
     }
  sr.setBit(0, LOW);    //turn off 1st ball
  for (int k = 0; k < lateZone; k++) {    //check for late button press 50% after 
       checkButtonLate();      
       delay(ballSpeed / 100);
     }
}


//check button press during "good" zone
void checkButtonGood() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonPress=1;
      analogWrite(greenHomePin, 0);
      //sr.setBit(7, HIGH);    //test to visualize button feedback
    }
}


//check button press during "late" zone
void checkButtonLate() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonPress=1;
      analogWrite(greenHomePin, 120);
      analogWrite(redHomePin, 120);
      //sr.setBit(6, HIGH);    //test to visualize button feedback
    }
}


void ballSpeedIncrease() {
  speedIncrease = ballSpeed / 10;
  ballSpeed = ballSpeed - speedIncrease;
  if (ballSpeed < 20) ballSpeed = 200;
}


//flashes all LEDs 50 times - button pressed too early
void endGameEarly() {            
  for (int k=0; k<50; k++){
    for (int i=0; i<8; i++) {
      sr.setBit(i, HIGH);
    }
    analogWrite(redHomePin, 0);
    delay(250);
    allOff();
    delay(250);
  }
}

//flashes all LEDs 50 times - button not pressed in time
void endGameLate() {            
  for (int k=0; k<50; k++){
    for (int i=0; i<8; i++) {
      sr.setBit(i, HIGH);
    }
    analogWrite(blueHomePin, 0);
    delay(50);
    allOff();
    delay(50);
  }
}

void allOff() {
  for (int i=0; i<8; i++) {
    sr.setBit(i, LOW);
    analogWrite(redHomePin, 255);
    analogWrite(greenHomePin, 255);
    analogWrite(blueHomePin, 255);
  }
}
