#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

/*
 Clyde Blink Ambient RGB Light
 
 Turns the ambient light red for one second, then off for one second
 then green and off and blue and off, repeatedly.
 
 */

// the setup routine runs once when you press reset:
void setup() {                
  // Initialize Clyde
  Wire.begin();
  Clyde.begin();  
}

// the loop routine runs over and over again forever:
void loop() {

  int r, g, b;

  //Red
  r=255, g=0, b=0;  // setting the r to 255, and g and b to 0 gives red.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

  //Off
  r=0, g=0, b=0; // setting the r, g and b to 255 turns it off.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

  //Green
  r=0, g=255, b=0;  // setting the g to 255, and r and b to 0 gives green.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

  //Off
  r=0, g=0, b=0; // setting the r, g and b to 0 turns it off.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

  //Blue
  r=0, g=0, b=255;// setting the b to 255, and r and g to 0 gives blue.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

  //Off
  r=0, g=0, b=0; // setting the r, g and b to 255 turns it off.
  Clyde.setAmbient(RGB(r, g, b)); 
  Clyde.updateAmbientLight();
  delay(1000);  // wait for a second

}


