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
  // Uncomment this line to talk to Clyde over the Serial Monitor
  //while (!Serial) ;
  sCmd.addCommand("TIME", digitalClockDisplay);

  setTime(8,29,0,1,1,11);
  Alarm.alarmRepeat(8,31,0, startSunrise);
  Alarm.alarmRepeat(8,33,0, startSunriseWhiteLight);
  Clyde.eeprom()->reset();
  Clyde.begin();
}

void loop() {
  //read the serial communication if any
  sCmd.readSerial();

  //calibrate the eye and check for press
  Clyde.updateEye();

#ifdef ENABLE_MOUTH
  //update the mouth to play sounds
  Clyde.updateMouth();
#endif
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();

  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();

  // check for alarm.
  // Note: the serviceAlarms function is private in the original TimeAlarms library and the
  // only way to check for an alarm was the delay function.
  Alarm.serviceAlarms();
  //Alarm.delay( 1 );  // delay for 1ms and during that time period check for alarms.
}

void startSunriseWhiteLight(){
  Clyde.fadeWhite( 0, 1000 );
}

void startSunrise(){
  RGB sunriseColors[7] = {RGB(19, 17, 28), RGB(39, 34, 57), RGB(78, 69, 114), RGB(46, 108, 181), RGB(168, 142, 127), RGB(255, 166, 48), RGB(255, 210, 66) };
  uint32_t sunriseIntervals[7] = {30000, 60000, 60000, 60000, 60000, 60000, 60000}; //real
  Clyde.setCycle( SUNRISE, 7, sunriseColors, sunriseIntervals, NO_LOOP );
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

