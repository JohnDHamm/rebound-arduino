
/*
"rebound" 2 player version
author: John Hamm
created 5.14.15

build 11 - reset loop bug, final score displays, start sequence

build 10 - runs loop bug, change to ballPosition variable

build 09 - speed bug fix, score displays

build 08 - start of 2 player version

*/

#include <ShiftRegister.h>    //uses shiftRegisterLib from Leonardo Banderali

ShiftRegister sr(1, 8, 7, 4, 12);
/*
-parameters:
  1 shift register
  pin 8 -> data
  pin 7 -> clock
  pin 4 -> latch
  pin 12 -> reset
*/

int buttonApin = 13;
int buttonBpin = 2;

int ballPosition = 0;      //start position of ball
int ballDirection = 1;      //start direction of ball 1 = A to B (left), 2 = B to A (right)
int startBallSpeed = 300;    //for resetting ball speed
int ballSpeed = 300;      //time between ball positions (make same as startBallSpeed)
int speedIncrease = 0;    //factor to increase speed

int buttonAPress = 0;    //track if button pushed in time 0=early/not at all, 1=good, 2=great, 3=late, but ok
int buttonBPress = 0;

int earlyZone = 50;    //% of ballSpeed interval where button press in rebound zone is too early
int goodZone = 25;    //% of ballSpeed interval where button press in rebound zone is good 
int greatZone = 25;    //% of ballSpeed interval where button press in rebound zone is great (early + good + great=100%=interval between led 7/0 and home)
int lateZone = 50;    //% of ballSpeed interval where button press in rebound zone is late, but accepted
int wayLateZone = 50;  //% of ballSpeed interval where button press in rebound zone is way late (late + wayLate = 100% interval between home and led 7/0)

int redBHomePin = 3;      //pin for homeRGB LED connected to red
int greenBHomePin = 5;    //pin for homeRGB LED connected to green
int blueBHomePin = 6;      //pin for homeRGB LED connected to blue

int redAHomePin = 9;      //pin for homeRGB LED connected to red
int greenAHomePin = 10;    //pin for homeRGB LED connected to green
int blueAHomePin = 11;      //pin for homeRGB LED connected to blue

int scoreA = 0;    //round scoring tabulation
int scoreB = 0;
int lastWinner = 1;    //winner of last round 1=A, 2=B
int reset = 0;        //value to reset ball position after each round

int winScore = 4;    //# of winning rounds needed to win game

//------------------------------setup---------------------------------

void setup() {

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);

  Serial.begin(9600);
   sr.setShiftOrder(MSBFIRST);  //set order in which bits are shifted (MSBFIRST/LSBFIRST)
  sr.clear();                  //set all bits to low

  while (! Serial); // Wait untilSerial is ready
  Serial.println("Ready to start...");        //serial output used throughout to verify gameplay

  allOff();
  delay(1000);
  startSeq();    //start sequence - signifies each round starting
}
  
//-----------------------------game loop---------------------------------------

void loop() {

  Serial.print("ball speed is ");
  Serial.println(ballSpeed);
  Serial.print(" position ");
  Serial.println(ballPosition);
  Serial.print(" direction ");
  Serial.println(ballDirection);
  
  
  if (ballPosition <= 6 && ballDirection == 1) {
    moveBallLeft();
  }
  
  if (ballPosition == 7 && ballDirection == 1) {
    Serial.print("ball speed is ");
    Serial.println(ballSpeed);
    Serial.print(" position ");
    Serial.println(ballPosition);
    Serial.print(" direction ");
    Serial.println(ballDirection);
    reboundBCheck();
  }
  
  if (ballPosition >= 1 && ballDirection == 2) {
    moveBallRight();
  }
  
  if (ballPosition == 0 && ballDirection == 2) {
    Serial.print("ball speed is ");
    Serial.println(ballSpeed);
    Serial.print(" position ");
    Serial.println(ballPosition);
    Serial.print(" direction ");
    Serial.println(ballDirection);
    reboundACheck();
  }
  
}


//------------------functions---------------------------

void moveBallLeft() {
  allOff();
  buttonAPress = 0;
  buttonBPress = 0;
  reset = 0;
  sr.setBit(ballPosition, HIGH);
  for (int k = 0; k < 10; k++) {
       checkBbad();      
       delay(ballSpeed / 10);
     }
  sr.setBit(ballPosition, LOW);
  if (reset == 0) {
    ballPosition = ballPosition + 1;
  }
}

void moveBallRight() {
  allOff();
  buttonAPress = 0;
  buttonBPress = 0;
  reset = 0;
  sr.setBit(ballPosition, HIGH);
  for (int k = 0; k < 10; k++) {
       checkAbad();      
       delay(ballSpeed / 10);
     }
  sr.setBit(ballPosition, LOW);
  if (reset == 0) {
    ballPosition = ballPosition - 1;
  }
}


//check bad button press during runs + early rebound
void checkAbad() {          
    if (digitalRead(buttonApin) == LOW) {
      Serial.println("player A too soon! ");
      scoreB = scoreB + 1;
      Awrong();
      buttonAPress = -1;
      lastWinner = 2;
      scoreCheck();
    }
}

void checkBbad() {          
    if (digitalRead(buttonBpin) == LOW) {
      Serial.println("player B too soon! ");
      scoreA = scoreA + 1;
      Bwrong();
      buttonBPress = -1;
      lastWinner = 1;
      scoreCheck();
    }
}


//check for B button press in the rebound "zone"
void reboundBCheck() {
  buttonAPress = 0;
  buttonBPress = 0;
  reset = 0;
  for (int k = 0; k < earlyZone; k++) {    //check 100x/interval for early button press 1st % of interval time
       if (reset == 0) {
         sr.setBit(7, HIGH);
         checkAbad();
         checkBbad();
       }     
       delay(ballSpeed / 100);
     } 
  for (int k = 0; k < goodZone; k++) {    //check for early button press in good % of interval time
       if (buttonBPress == 0) {
          checkButtonBGood();
        }      
       delay(ballSpeed / 100);
     }
  for (int k = 0; k < greatZone; k++) {    //check for early button press in great % of interval time
       if (buttonBPress == 0) {
          checkButtonBGreat();
        }      
       delay(ballSpeed / 100);
     }
  sr.setBit(7, LOW);    //turn off 1st ball
  for (int k = 0; k < lateZone; k++) {    //check for late button press % after 
       if (buttonBPress == 0) {
          checkButtonBLate(); 
        }           
       delay(ballSpeed / 100);
     }
     
  /*for (int k = 0; k < wayLateZone; k++) {    //check for way late button press % after 
       if (buttonBPress == 0) {
          checkButtonBWayLate();
        }           
       delay(ballSpeed / 100);
     }*/
     
  if (buttonBPress == 0) {          //button never pressed
    Serial.println("player B did not try! ");
    scoreA = scoreA + 1;
    Bwrong();
    scoreCheck();
  }
  speedCheckB();
}


//check button press during "good" zone
void checkButtonBGood() {      
    if (digitalRead(buttonBpin) == LOW) {
      buttonBPress = 1;
      analogWrite(greenBHomePin, 120);
      analogWrite(blueBHomePin, 0);
      Serial.println("player B good! ");
      ballDirection = 2;
    }
}

//check button press during "great" zone
void checkButtonBGreat() {      
    if (digitalRead(buttonBpin) == LOW) {
      buttonBPress = 2;
      analogWrite(greenBHomePin, 0);
      Serial.println("player B great! ");
      ballDirection = 2;
    }
}


//check button press during "late" zone
void checkButtonBLate() {      
    if (digitalRead(buttonBpin) == LOW) {
      buttonBPress = 3;
      analogWrite(greenBHomePin, 120);
      analogWrite(redBHomePin, 120);
      Serial.println("player B late, but still good! ");
      ballDirection = 2;
    }
}

//check button press during "way late" zone
void checkButtonBWayLate() {      
    if (digitalRead(buttonBpin) == LOW) {
      //buttonBPress = 4;
      analogWrite(redBHomePin, 0);
      Serial.println("player B way too late! ");
      scoreA = scoreA + 1;
      Bwrong();
      scoreCheck();
    }
}

//check for A button press in the rebound "zone"
void reboundACheck() {
  buttonAPress = 0;
  buttonBPress = 0;
  reset = 0;
  for (int k = 0; k < earlyZone; k++) {    //check 100x/interval for early button press 1st % of interval time      
       if (reset == 0) {
         sr.setBit(0, HIGH);
         checkAbad();
         checkBbad();
       }    
       delay(ballSpeed / 100);
     } 
  for (int k = 0; k < goodZone; k++) {    //check for early button press in good % of interval time
       if (buttonAPress == 0) {
          checkButtonAGood();
        }      
       delay(ballSpeed / 100);
     }
  for (int k = 0; k < greatZone; k++) {    //check for early button press in great % of interval time
       if (buttonAPress == 0) {
          checkButtonAGreat();
        }      
       delay(ballSpeed / 100);
     }
  sr.setBit(0, LOW);    //turn off 1st ball
  for (int k = 0; k < lateZone; k++) {    //check for late button press % after 
       if (buttonAPress == 0) {
          checkButtonALate(); 
        }           
       delay(ballSpeed / 100);
     }
  
  /*for (int k = 0; k < wayLateZone; k++) {    //check for way late button press % after 
       if (buttonAPress == 0) {
          checkButtonAWayLate();
        }           
       delay(ballSpeed / 100);
     }*/
     
  if (buttonAPress == 0) {        //button never pressed
    Serial.println("player A did not try! ");
    scoreB = scoreB + 1;
    Awrong();
    scoreCheck();
  }
  speedCheckA();
}


//check button press during "good" zone
void checkButtonAGood() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonAPress = 1;
      analogWrite(greenAHomePin, 120);
      analogWrite(blueAHomePin, 0);
      Serial.println("player A good! ");
      ballDirection = 1;
    }
}

//check button press during "great" zone
void checkButtonAGreat() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonAPress = 2;
      analogWrite(greenAHomePin, 0);
      Serial.println("player A great! ");
      ballDirection = 1;
    }
}


//check button press during "late" zone
void checkButtonALate() {      
    if (digitalRead(buttonApin) == LOW) {
      buttonAPress = 3;
      analogWrite(greenAHomePin, 120);
      analogWrite(redAHomePin, 120);
      Serial.println("player A late, but still good! ");
      ballDirection = 1;
    }
}

//check button press during "way late" zone
void checkButtonAWayLate() {      
    if (digitalRead(buttonApin) == LOW) {
      //buttonAPress = 4;
      analogWrite(redAHomePin, 0);
      Serial.println("player A way too late! ");
      scoreB = scoreB + 1;
      Awrong();
      scoreCheck();
    }
    
}

void speedCheckB() {
  
  if (buttonBPress == 1) {
    speedIncrease = ballSpeed / 10;        //increase ballSpeed by 10% if in good zone
    ballSpeed = ballSpeed - speedIncrease;
  }
  if (buttonBPress == 2) {
    speedIncrease = ballSpeed / 5;        //increase ballSpeed by 20% if in great zone
    ballSpeed = ballSpeed - speedIncrease;
  }
  if (buttonBPress == 3) {
    speedIncrease = ballSpeed / 5;        //decrease ballSpeed by 20% if in late zone
    ballSpeed = ballSpeed + speedIncrease;
  }
}

void speedCheckA() {
  
  if (buttonAPress == 1) {
    speedIncrease = ballSpeed / 10;        //increase ballSpeed by 10% if in good zone
    ballSpeed = ballSpeed - speedIncrease;
  }
  if (buttonAPress == 2) {
    speedIncrease = ballSpeed / 5;        //increase ballSpeed by 20% if in great zone
    ballSpeed = ballSpeed - speedIncrease;
  }
  if (buttonAPress == 3) {
    speedIncrease = ballSpeed / 5;        //decrease ballSpeed by 20% if in late zone
    ballSpeed = ballSpeed + speedIncrease;
  }
}

void scoreCheck() {      //display scores, check for winner, start next round
  allOff();
  for (int i=0; i < scoreA; i++) {
    sr.setBit(i, HIGH);
    delay (1000);
  }
  Serial.print("player A score: ");
  Serial.println(scoreA);
  if (scoreB > 0) {
    for (int i=0; i < scoreB; i++) {
      sr.setBit(7-i, HIGH);
      delay (1000);
    }
  }
  Serial.print("player B score: ");
  Serial.println(scoreB);
  if (scoreA == winScore) {
    Awins();
  }
  if (scoreB == winScore) {
    Bwins();
  }
  delay (2000);
  allOff();
  delay (500);
  Serial.print("NEXT ROUND... ");
  startSeq();
  resetVars();
}


void resetVars() {
  ballSpeed = startBallSpeed;
  if (lastWinner == 1) {
    ballPosition = 0;
    ballDirection = 1;
  }
  if (lastWinner == 2) {
    ballPosition = 7;
    ballDirection = 2;
  }
  reset = 1;
  
}

    
void Awins() {
  Serial.print("player A wins!");
  //allOff();
  for (int k = 0; k < 20; k ++) {
    for (int i = 255; i > -1; i--) {
      analogWrite(greenAHomePin, i);
      delay (1);
    }
    for (int i = 0; i < 256; i++) {
      analogWrite(greenAHomePin, i);
      delay (1);
    }
  }
}

void Bwins() {
  Serial.print("player B wins!");
  //allOff();
  for (int k = 0; k < 20; k ++) {
    for (int i = 255; i > -1; i--) {
      analogWrite(greenBHomePin, i);
      delay (1);
    }
    for (int i = 0; i < 256; i++) {
      analogWrite(greenBHomePin, i);
      delay (1);
    }
  }
}


void Awrong() {    //flashes A red and B green
  allOff();
  lastWinner = 2;
  for (int k = 0; k < 3; k ++) {
    for (int i = 255; i > -1; i--) {
      analogWrite(greenBHomePin, i);
      analogWrite(redAHomePin, i);
      delay (2);
    }
    for (int i = 0; i < 256; i++) {
      analogWrite(greenBHomePin, i);
      analogWrite(redAHomePin, i);
      delay (2);
    }
  }
  allOff();
  delay(500);
}

void Bwrong() {    //flashes B red and A green
  allOff();
  lastWinner = 1;
  for (int k = 0; k < 3; k ++) {
    for (int i = 255; i > -1; i--) {
      analogWrite(greenAHomePin, i);
      analogWrite(redBHomePin, i);
      delay (2);
    }
    for (int i = 0; i < 256; i++) {
      analogWrite(greenAHomePin, i);
      analogWrite(redBHomePin, i);
      delay (2);
    }
  }
  allOff();
  delay(500);
}


//turns all LEDs off
void allOff() {
  for (int i=0; i<8; i++) {
    sr.setBit(i, LOW);
    analogWrite(redAHomePin, 255);
    analogWrite(greenAHomePin, 255);
    analogWrite(blueAHomePin, 255);
    analogWrite(redBHomePin, 255);
    analogWrite(greenBHomePin, 255);
    analogWrite(blueBHomePin, 255);
  }
}



//start sequence - flash home A or B led blue 4x, green 1x at start speed
void startSeq() {        
  allOff();
  if (ballDirection == 1) {
    for (int i = 0; i < 4; i++) {
      analogWrite(blueAHomePin, 0);
      delay (startBallSpeed);
      allOff();
      delay (startBallSpeed);
    }
    analogWrite(greenAHomePin, 0);
    delay (startBallSpeed);
    allOff();
    delay (startBallSpeed);
  }
    if (ballDirection == 2) {
    for (int i = 0; i < 4; i++) {
      analogWrite(blueBHomePin, 0);
      delay (startBallSpeed);
      allOff();
      delay (startBallSpeed);
    }
    analogWrite(greenBHomePin, 0);
    delay (startBallSpeed);
    allOff();
    delay (startBallSpeed);
  }
}

/*
// testing all LED connections 
void testRun() {
  for (int i = 0; i < 8; i++) {  
       sr.setBit(i, HIGH);    
       delay(ballSpeed);       
    sr.setBit(i, LOW);              
  }
  analogWrite(redBHomePin, 0);
  delay(ballSpeed);
  analogWrite(redBHomePin, 255);
  analogWrite(greenBHomePin, 0);
  delay(ballSpeed);
  analogWrite(greenBHomePin, 255);
  analogWrite(blueBHomePin, 0);
  delay(ballSpeed);
  analogWrite(blueBHomePin, 255);
  
  for (int i = 7; i > -1; i--) {
      sr.setBit(i, HIGH); 
      delay(ballSpeed);    
    sr.setBit(i, LOW);
  }
  analogWrite(redAHomePin, 0);
  delay(ballSpeed);
  analogWrite(redAHomePin, 255);
  analogWrite(greenAHomePin, 0);
  delay(ballSpeed);
  analogWrite(greenAHomePin, 255);
  analogWrite(blueAHomePin, 0);
  delay(ballSpeed);
  analogWrite(blueAHomePin, 255);
  delay(ballSpeed);
}
*/
