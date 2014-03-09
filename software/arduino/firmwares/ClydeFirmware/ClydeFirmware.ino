#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Clyde.eeprom()->reset();
  Clyde.begin();
}

void loop() {
  //read the serial communication if any
  Clyde.readSerial();
  
  //calibrate the eye and check for press
  Clyde.updateEye();
  
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();
  
  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();
}
