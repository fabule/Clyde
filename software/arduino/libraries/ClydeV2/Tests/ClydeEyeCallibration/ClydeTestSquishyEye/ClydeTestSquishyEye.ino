#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>


#define FIRMWARE_VERSION 2

int testNumber = 1;
boolean stayInCurrentTest = false;//Some functionality requires multiple trips through the loop.  Use this flag to control that.

void setup() {
  Wire.begin();
  Clyde.begin();

  delay(2000);

  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Test SquishyEye");
  Serial.println();
  

  
}

void loop() {
  
    Clyde.squishyEye.update();
  
/*
  switch(testNumber){
  case 1:
    //setBrightness
    Serial.println("1 setBrightness 0");
    Clyde.taskLight.setBrightness(0);

    break;

  case 9:
    //fadeTo  OFF
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      Serial.println("");

      stayInCurrentTest = true;
    }
    else{
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.taskLight.brightness>=255){
      stayInCurrentTest = false;
    }
    break;

  default:
    testNumber = 0;
  }

  if(!stayInCurrentTest){
    delay(1000);
    testNumber++;
    Serial.println();
  }
  
  */

}
