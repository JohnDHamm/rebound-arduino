
/*
rebound build 07 - finalize scoring + display


build 06 - add RGB LED with result feedback
  + correct button being pressed for period of time (ignore after 1st press)
  + start sequence
  + add "great" zone/alter "good" zone
  +add serial feedback of zone + speed
  
  build 05 - 1 player game setup (no brightness controls of LED runs)

*/

#include <ShiftRegister.h>

ShiftRegister sr(1, 8, 10, 9, 12);

int buttonApin = 13;

int ballSpeed = 300;    //initial time between ball positions
int speedIncrease = 0;    //factor to increase speed

//int turn=1;          //keep track of how many "turns" (runs)
int score=0;          //scoring value

int buttonPress = 0;    //track if button pushed in time 0=no, 1=good, 2=great

int earlyZone = 50;    //% of ballSpeed where button press in rebound zone is too early
int goodZone = 25;    //% of ballSpeed where button press in rebound zone is good (early + good =100=interval between led 0 and home)
int greatZone = 25;
int lateZone = 50;    //% of ballSpeed where button press in rebound zone is late, but accepted

int redHomePin = 3;      //pin for homeRGB LED connected to red
int greenHomePin = 5;    //pin for homeRGB LED connected to red
int blueHomePin = 6;      //pin for homeRGB LED connected to red

int scoreTens = 0;
int scoreOnes = 0;
int counter = 0;    //for score display calculation


//------------------------------setup---------------------------------

void setup() {

  pinMode(buttonApin, INPUT_PULLUP);

Serial.begin(9600);
  sr.setShiftOrder(MSBFIRST);  //set order in which bits are shifted (MSBFIRST/LSBFIRST)
  sr.clear();                  //set all bits to low


//start sequence - home LED blinks purple 3x, then green once
while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Ready to start...");

allOff();
delay(1000);

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

  Serial.print("ball speed is ");
  Serial.println(ballSpeed);
  runUpDown();    
  reboundCheck();
  if (buttonPress == 0) {
    endGameLate();
  }
  if (buttonPress == 1) {
    ballSpeedIncrease();
  }
  if (buttonPress == 2) {
    ballSpeedDecrease();
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
      Serial.println("too soon! ");
      //Serial.println(k);
      endGameEarly();
    }
}


//check for button press in the rebound "zone"
void reboundCheck() {
  for (int k = 0; k < earlyZone; k++) {    //check for early button press 1st % of interval time
       sr.setBit(0, HIGH);
       checkButtonBad();      
       delay(ballSpeed / 100);
     } 
  for (int k = 0; k < goodZone; k++) {    //check for early button press in good % of interval time
       if (buttonPress == 0) {
          checkButtonGood();
        }      
       delay(ballSpeed / 100);
     }
  for (int k = 0; k < greatZone; k++) {    //check for early button press in great % of interval time
       if (buttonPress == 0) {
          checkButtonGreat();
        }      
       delay(ballSpeed / 100);
     }
  sr.setBit(0, LOW);    //turn off 1st ball
  for (int k = 0; k < lateZone; k++) {    //check for late button press % after 
       if (buttonPress == 0) {
          checkButtonLate(); 
        }           
       delay(ballSpeed / 100);
     }
}


//check button press during "good" zone
void checkButtonGood() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonPress=1;
      score = score + 1;
      analogWrite(greenHomePin, 120);
      analogWrite(blueHomePin, 0);
      Serial.println("good! ");
      Serial.print("score: ");
      Serial.println(score);
    }
}

//check button press during "great" zone
void checkButtonGreat() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonPress=2;
      score = score + 3;
      analogWrite(greenHomePin, 0);
      Serial.println("great! ");
      Serial.print("score: ");
      Serial.println(score);
    }
}


//check button press during "late" zone
void checkButtonLate() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonPress=1;
      analogWrite(greenHomePin, 120);
      analogWrite(redHomePin, 120);
      Serial.println("late, but still good! ");
      //sr.setBit(6, HIGH);    //test to visualize button feedback
    }
}


void ballSpeedIncrease() {
  speedIncrease = ballSpeed / 10;        //increase ballSpeed by 10% if in good zone, or late zone
  ballSpeed = ballSpeed - speedIncrease;
  //if (ballSpeed < 20) ballSpeed = 200;
}

void ballSpeedDecrease() {
  speedIncrease = ballSpeed / 10;       
  ballSpeed = ballSpeed + speedIncrease;     //decrease ballSpeed by 10% if in good zone, or late zone
  //if (ballSpeed < 20) ballSpeed = 200;
}




//flashes all LEDs 50 times - button pressed too early
void endGameEarly() {
  Serial.print("Game over. Final score: ");
  Serial.println(score);
  scoreDisplay();
  /*for (int k=0; k<50; k++){
    for (int i=0; i<8; i++) {
      sr.setBit(i, HIGH);
    }
    analogWrite(redHomePin, 0);
    delay(250);
    allOff();
    delay(250);
  }  */
}

//flashes all LEDs 50 times - button not pressed in time
void endGameLate() {
  Serial.println("too late sucker!");
  Serial.print("Game over. Final score: ");
  Serial.println(score);  
  scoreDisplay();
  /*for (int k=0; k<50; k++){
    for (int i=0; i<8; i++) {
      sr.setBit(i, HIGH);
    }
    analogWrite(blueHomePin, 0);
    delay(50);
    allOff();
    delay(50);
  }  */
}

void allOff() {
  for (int i=0; i<8; i++) {
    sr.setBit(i, LOW);
    analogWrite(redHomePin, 255);
    analogWrite(greenHomePin, 255);
    analogWrite(blueHomePin, 255);
  }
}

//signify end of game, then display score
void scoreDisplay() {
  for (int k=0; k<10; k++){      //flash all LEDs + home Red 10x
    for (int i=0; i<8; i++) {
      sr.setBit(i, HIGH);
    }
    analogWrite(redHomePin, 0);
    delay(50);
    allOff();
    delay(50);
  }
  
  delay (1000);
  
  for (int k=1; k<10; k++) {    //calculate how many "tens" and "ones" in score
      counter=k*10;
      if (score > counter -1) {
      scoreTens = counter/10;
      scoreOnes = score - counter;
      }
      if (score < 10) {
        scoreOnes = score;
      }
  }
  Serial.print("scoreTens: ");
  Serial.println(scoreTens);  
  Serial.print("scoreOnes: ");
  Serial.println(scoreOnes);  
  
  score = 0;    //reset score for reset button
  ballSpeed = 300;    //reset for testing
  
  for (int k = 0; k <3; k++) {          //display score 3x
    for (int i=0; i<scoreTens; i++) {    //display "tens" as green home flashes
      analogWrite(greenHomePin, 0);
      delay(500);
      allOff();
      delay(500);
    }
    if (scoreOnes > 0) {
      for (int i=7; i>7-scoreOnes; i--) {    //display "ones" as red leds from 8 to 0
        sr.setBit(i, HIGH);
        delay(500);
      }
    }
    if (scoreOnes == 9) {                  //if ones=9, use redHome for 9th led
      analogWrite(redHomePin, 0);
      delay (500);
    }
    delay (1000);
    allOff();
    delay(500);
  }
  
  scoreTens = 0;      //reset for testing
  scoreOnes = 0;      //   ""
}
  
