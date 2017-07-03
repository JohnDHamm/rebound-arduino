

// scratchPad runUp of 8 LEDs non_PWM fading
void runUp() {
  int j=0;    //for ballPos array
  int b=0;  //for brightness array
  int brightnessPin = ?; //pin connected to SR pin 13 OE
  
  int numLEDs[]={1,2,3,4,4,4,4,4};
  int brightness[]={255,fade1,fade2,0};
  int ballPos[]={0,1,2,3,4,5,6,7};
  
  for (int m=0; m<8; m++); {
    j=m;
    b=0;
    for (int i=0; i<numLEDs[m]; i++) {
      allOff();
      analogWrite(brightnessPin, brightness[b]);
      sr.setBit(ballPos[j], HIGH);
      checkButton();
      j--;
      b++;
      delay?    //
    }
  }
}
      
