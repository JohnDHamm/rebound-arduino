/*
rebound test build 01 - testing button functionality + controlling brightness of red LEDs

*/

int led1Pin = 10;
int buttonApin = 12;

byte leds = 0;

//---------setup----------------------------------------------------------------------------
void setup() 
{
  pinMode(led1Pin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);    
}

//---------main loop-----------------------------------------------------------------------
void loop() 
{
  if (digitalRead(buttonApin) == LOW) {
    led1fade();
    }
}


//-----functions----------------------------------------------------------------------------
void setBrightness(byte brightness) // 0 to 255
{
  analogWrite(led1Pin, brightness);
}


void led1fade() {
  for (byte b = 255; b > 0; b--) {
      setBrightness(b);
      delay(2);
  }
  setBrightness(0);
}

