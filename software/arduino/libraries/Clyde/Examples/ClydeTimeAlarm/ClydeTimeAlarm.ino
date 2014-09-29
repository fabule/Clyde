#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>
#include <Time.h>
#include <TimeAlarms.h>

/**
  This is an alternative firmware that enables an alarm that will cause Clyde to
  switch on the light at the specified time.
 */

#define FIRMWARE_VERSION 1

SerialCommand sCmd;

void setup() {
  Wire.begin();

  Serial.begin(9600);
  // Uncomment this line to talk to Clyde over the Serial Monitor
  //while (!Serial) ;
  sCmd.addCommand("TIME", digitalClockDisplay);

  // set the time below to the current time when compiling and uploading
  setTime(20,15,0,2,9,14);
  
  // example alarms.
  Alarm.alarmRepeat(6,15,0, startSunrise);
  Alarm.alarmRepeat(6,18,0, startSunriseWhiteLight);
  Clyde.eeprom()->reset();
  Clyde.begin();
}

void loop() {
  //read the serial communication if any
  sCmd.readSerial();

#ifdef ENABLE_EYE
  //calibrate the eye and check for press
  Clyde.updateEye();
#endif
 
#ifdef ENABLE_MOUTH
  //update the mouth to play sounds
  Clyde.updateMouth();
#endif
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();

#ifdef ENABLE_EYE
  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();
#else
  Clyde.updatePersonalities();
#endif
  
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

