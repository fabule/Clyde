#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Clyde.begin();
  //Clyde.eeprom()->reset();
}

void loop() {
  //TODO break this update down here to be easier to modify
  Clyde.update();
}
