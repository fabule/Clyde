#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Clyde.begin();
  
  //uncomment this line if you want to
  //reset the eeprom to the factory default
  //Clyde.eeprom()->reset();
}

void loop() {
  //TODO break this update down here to be easier to modify
  Clyde.update();
}
