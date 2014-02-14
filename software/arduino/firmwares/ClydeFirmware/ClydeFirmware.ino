#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Clyde.begin();
}

void loop() {
  //calibrate the eye and check for press
  Clyde.updateEye();
  
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();
  
  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();
}
