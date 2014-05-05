#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

SerialCommand sCmd;

void setup() {
  Wire.begin();
  
  Serial.begin(9600);
  sCmd.addCommand("SERIAL", cmdSerial);
  sCmd.addCommand("VERSION", cmdVersion);
  
  //Clyde.eeprom()->reset();
  Clyde.begin();
}

void loop() {
  //read the serial communication if any
  sCmd.readSerial();
  
  //calibrate the eye and check for press
  Clyde.updateEye();
  
  //update the mouth to play sounds
  Clyde.updateMouth();
  
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();
  
  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();
}

//
// Serial Commands
//

void cmdSerial() {
  char serial[7] = {0};
  Clyde.eeprom()->readSerial(&serial[0]);  
  Serial.print("OK "); 
  Serial.println(serial);
}

void cmdVersion() {
  uint16_t vers = 0;
  Clyde.eeprom()->readVersion(&vers);
  Serial.print("OK "); 
  Serial.println(vers);
}
