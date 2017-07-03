/*
rebound build 03 - rundown of lights back to start

    build 02 - running lights/increase speed with button

   build 01 - testing button functionality + controlling brightness of red LEDs

*/

//int led1Pin = 10;
int buttonApin = 12;

byte leds = 0;

int cycle = 100;
int fade1 = 50;
int fade2 = 5;
  
  
//---------setup----------------------------------------------------------------------------
void setup() 
{
  //pinMode(led1Pin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);    
}

//---------main loop-----------------------------------------------------------------------
void loop() 
{
  if (digitalRead(buttonApin) == LOW) {
     //led1fade();
     runup();
     cycle = cycle - 5;      //increase speed
     if (cycle < 0) cycle = 100;
    }
}


//-----functions----------------------------------------------------------------------------
/*void setBrightness(byte brightness) // 0 to 255
{
  analogWrite(led1Pin, brightness);
}
*/

/*void led1fade() {
  for (byte b = 255; b > 0; b--) {
      setBrightness(b);
      delay(2);
  }
  setBrightness(0);
}
*/

void runup() {            //led1=pin10, led2=pin9, led3=pin6, led4=pin5, led5=pin3
  analogWrite(10, 255);
  delay(cycle);
  analogWrite(9, 255);
  analogWrite(10, fade1);
  delay(cycle);
  analogWrite(6, 255);
  analogWrite(9, fade1);
  analogWrite(10, fade2);
  delay(cycle);
  analogWrite(5, 255);
  analogWrite(6, fade1);
  analogWrite(9, fade2);
  analogWrite(10, 0);
  delay(cycle);
  analogWrite(3, 255);
  analogWrite(5, fade1);
  analogWrite(6, fade2);
  analogWrite(9, 0);
  delay(cycle);
  analogWrite(3, fade1);
  analogWrite(5, 255);
  analogWrite(6, 0);
  delay(cycle);
  analogWrite(3, fade2);
  analogWrite(5, fade1);
  analogWrite(6, 255);
  delay(cycle);
  analogWrite(3, 0);
  analogWrite(5, fade2);
  analogWrite(6, fade1);
  analogWrite(9, 255);
  delay(cycle);
  analogWrite(5, 0);
  analogWrite(6, fade2);
  analogWrite(9, fade1);
  analogWrite(10, 255);
  delay(cycle);
  analogWrite(6, 0);
  analogWrite(9, fade2);
  analogWrite(10, fade1);
  delay(cycle);
  analogWrite(9, 0);
  analogWrite(10, fade2);
  delay(cycle);
  analogWrite(10, 0);
  
}
  
  
  
  
  
  
