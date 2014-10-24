#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

/* 
 Clyde Blink White Task Light
 Turns on the white task light for one second, then off for one second, repeatedly. 
*/

// the setup routine runs once when you press reset:
void setup() {                
  // Initialize Clyde
  Wire.begin();
  Clyde.begin();  
}

// the loop routine runs over and over again forever:
void loop() {

  //OFF
  Clyde.setWhite(255);  // setting the white light to 255 turns it OFF.
  Clyde.updateWhiteLight();
  delay(1000);  // wait for a second, or 1000 milliseconds

  //On
  Clyde.setWhite(0); // setting the white light to zero turns it ON.
  Clyde.updateWhiteLight();
  delay(1000);  // wait for a second, or 1000 milliseconds
}
