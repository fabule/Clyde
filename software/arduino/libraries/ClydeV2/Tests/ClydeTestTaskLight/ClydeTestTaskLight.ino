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
  Serial.println("Clyde Test TaskLight");
  Serial.println();
}

void loop() {

  switch(testNumber){
  case 1:
    //setBrightness
    Serial.println("1 setBrightness 0");
    Clyde.taskLight.setBrightness(0);

    break;

  case 2:
    //setBrightness
    Serial.println("2 setBrightness 125");
    Clyde.taskLight.setBrightness(125);
    break;

  case 3:
    //setBrightness
    Serial.println("3 setBrightness 255");
    Clyde.taskLight.setBrightness(255);
    break;

  case 4:
    Serial.println("4 turnOn");
    //turnOn
    Clyde.taskLight.turnOn();
    break;

  case 5:
    //isOn
    Serial.println("5 isOn - it should be");
    Serial.println(Clyde.taskLight.isOn());
    break;

  case 6:
    //turnOff
    Serial.println("6 turnOff");
    Clyde.taskLight.turnOff();
    break;

  case 7:
    //isOn
    Serial.println("7 isOn - it shouldn't be");
    Serial.println(Clyde.taskLight.isOn());
    break;

  case 8:
    //fadeTo  ON
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      Serial.println("8 fadeTo ON / update");
      Clyde.taskLight.fadeTo(0, 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.taskLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.taskLight.brightness<=0){
      stayInCurrentTest = false;
    }

    break;

  case 9:
    //fadeTo  OFF
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      Serial.println("9 fadeTo OFF / update");
      Clyde.taskLight.fadeTo(255, 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.taskLight.update();
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

}













