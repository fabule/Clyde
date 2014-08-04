#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

#define FIRMWARE_VERSION 1

SerialCommand sCmd;

void setup() {
  Wire.begin();
  
  Serial.begin(9600);
  sCmd.addCommand("SERIAL", cmdSerial);
  sCmd.addCommand("VERSION", cmdVersion);
  sCmd.addCommand("RESET", cmdReset);
  sCmd.addCommand("SET_AMBIENT", cmdSetAmbient);
  sCmd.addCommand("SET_WHITE", cmdSetWhite);
  sCmd.addCommand("WRITE_EEPROM", cmdWriteEEPROM);
  sCmd.addCommand("READ_EEPROM", cmdReadEEPROM);
  
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
  uint16_t vers = FIRMWARE_VERSION;
  Serial.print("OK "); 
  Serial.println(vers);
}

void cmdReset() {
  Clyde.eeprom()->reset();
  Serial.println("OK"); 
}

void cmdSetAmbient() {
  char *param1, *param2, *param3;
  int r, g, b;
  
  //Get arguments
  param1 = sCmd.next();    // Red
  param2 = sCmd.next();    // Green
  param3 = sCmd.next();    // Blue
  r = atoi(param1);
  g = atoi(param2);
  b = atoi(param3);
  
  Clyde.setAmbient(RGB(r, g, b));
  Serial.println("OK");
}

void cmdSetWhite() {
  char *param1;
  int w;
  
  //Get arguments
  param1 = sCmd.next();    // Brightness
  w = atoi(param1);
  
  if (w > 255) w = 255;
  w = 255 - w;
  
  Clyde.setWhite(w);
  Serial.println("OK");
}

void cmdWriteEEPROM() {
  char *param1, *param2;
  int addr;
  byte value;
  
  //Get arguments
  param1 = sCmd.next();    // Address
  param2 = sCmd.next();    // Value
  addr = atoi(param1);
  value = atoi(param2);
  
  EEPROM.write(addr, value);
  Serial.println("OK");
}

void cmdReadEEPROM() {
  char *param1;
  int addr;
  
  //Get arguments
  param1 = sCmd.next();    // Address
  addr = atoi(param1);  
  
  Serial.print("OK ");
  Serial.println(EEPROM.read(addr));
}
