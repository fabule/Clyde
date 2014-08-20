#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>
#include <Time.h>
#include <TimeAlarms.h>

#define FIRMWARE_VERSION 1

SerialCommand sCmd;

void setup() {
  Wire.begin();

  Serial.begin(9600);
  sCmd.addCommand("SERIAL", cmdSerial);
  sCmd.addCommand("VERSION", cmdVersion);
  sCmd.addCommand("RESET", cmdReset);
  sCmd.addCommand("TIME", digitalClockDisplay);
//  sCmd.addCommand("SET_AMBIENT", cmdSetAmbient);
//  sCmd.addCommand("SET_WHITE", cmdSetWhite);
//  sCmd.addCommand("WRITE_EEPROM", cmdWriteEEPROM);
//  sCmd.addCommand("READ_EEPROM", cmdReadEEPROM);

  setTime(8,29,0,1,1,11);
  Alarm.alarmRepeat(8,31,0, MorningAlarm);
  Clyde.eeprom()->reset();
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

  // check for alarm.
  // Note: if this delay function causes any problems we make the serviceAlarms function
  // public and directly call that here.
  Alarm.delay( 5 );  // delay for 5ms.
}

void MorningAlarm(){
  Serial.println("Alarm: - turn lights on!");
  Clyde.setWhite(200);
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
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

/**
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
*/
