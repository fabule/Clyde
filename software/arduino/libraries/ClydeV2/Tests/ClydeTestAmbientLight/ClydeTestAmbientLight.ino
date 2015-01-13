#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>


#define FIRMWARE_VERSION 2

int testNumber = 1;
boolean stayInCurrentTest = false;//Some functionality requires multiple trips through the loop.  Use this flag to control that.
float hue = 0.0;


void setup() {
  Wire.begin();
  Clyde.begin();

  delay(2000);

  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Test AmbientLight");
  Serial.println();
}

void loop() {

  switch(testNumber){
    //RGB COLOR SPACE
  case 1:
    //setColor RGB
    Serial.println("1 setColor RGB red");
    Clyde.ambientLight.setColor(RGB(255,0,0));

    break;

  case 2:
    //setColor RGB
    Serial.println("2 setColor RGB green");
    Clyde.ambientLight.setColor(RGB(0,255,0));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 3:
    //setColor RGB
    Serial.println("3 setColor RGB blue");
    Clyde.ambientLight.setColor(RGB(0,0,255));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 4:
    //setColor RGB
    Serial.println("4 setColor RGB white");
    Clyde.ambientLight.setColor(RGB(255,255,255));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 5:
    //setColor RGB
    Serial.println("5 setColor RGB off");
    Clyde.ambientLight.setColor(RGB(0,0,0));
    Serial.println("isOn - it shouldn't be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 6:
    //turnOn
    Serial.println("6 turnOn");
    Clyde.ambientLight.turnOn();
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 7:
    //turnOff
    Serial.println("7 turnOff");
    Clyde.ambientLight.turnOff();
    Serial.println("isOn - it shouldn't be");
    Serial.println(Clyde.taskLight.isOn());
    break;

  case 8:
    //save
    Serial.println("8 save");
    //set color to magento 
    Clyde.ambientLight.setColor(RGB(255,0,255));
    //save
    Clyde.ambientLight.save();
    //turn off
    Clyde.ambientLight.turnOff();
    //turn on, should be magento
    Clyde.ambientLight.turnOn();
    Serial.println("isOn - it should be, and it should be magento");
    Serial.println(Clyde.ambientLight.isOn());
    break;


    //HSV COLOR SPACE
  case 9:
    //setColor HSV
    Serial.println("9 setColor HSV red");
    Clyde.ambientLight.setColor(HSV(0,1,255));

    break;

  case 10:
    //setColor HSV
    Serial.println("10 setColor HSV green");
    Clyde.ambientLight.setColor(HSV(0.33,1,255));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 11:
    //setColor HSV
    Serial.println("11 setColor HSV blue");
    Clyde.ambientLight.setColor(HSV(0.67,1,255));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 12:
    //setColor HSV
    Serial.println("12 setColor HSV white");
    Clyde.ambientLight.setColor(HSV(0,0,255));
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 13:
    //setColor HSV
    Serial.println("13 setColor HSV off");
    Clyde.ambientLight.setColor(HSV(0,0,0));
    Serial.println("isOn - it shouldn't be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 14:
    //turnOn
    Serial.println("14 turnOn");
    Clyde.ambientLight.turnOn();
    Serial.println("isOn - it should be");
    Serial.println(Clyde.ambientLight.isOn());
    break;

  case 15:
    //turnOff
    Serial.println("15 turnOff");
    Clyde.ambientLight.turnOff();
    Serial.println("isOn - it shouldn't be");
    Serial.println(Clyde.taskLight.isOn());
    break;

  case 16:
    //save
    Serial.println("16 save");
    //set color to magento 
    Clyde.ambientLight.setColor(HSV(0.83,1,255));
    //save
    Clyde.ambientLight.save();
    //turn off
    Clyde.ambientLight.turnOff();
    //turn on, should be magento
    Clyde.ambientLight.turnOn();
    Serial.println("isOn - it should be, and it should be magento");
    Serial.println(Clyde.ambientLight.isOn());
    break;


  case 17:
    //fadeTo RGB RED, from cyan
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to Cyan
      Clyde.ambientLight.setColor(RGB(0,255,255));
      delay(500);
      Serial.println("17 fadeTo red from cyan rgb / update");
      Clyde.ambientLight.fadeTo(RGB(255,0, 0), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.redLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

  case 18:
    //fadeTo RGB GREEN, from red
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to Cyan
      Clyde.ambientLight.setColor(RGB(255,0,0));
      delay(500);
      Serial.println("18 fadeTo green from red rgb / update");
      Clyde.ambientLight.fadeTo(RGB(0,255, 0), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.greenLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

  case 19:
    //fadeTo RGB BLUE, from green
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to Cyan
      Clyde.ambientLight.setColor(RGB(0,255,0));
      delay(500);
      Serial.println("19 fadeTo blue from green rgb / update");
      Clyde.ambientLight.fadeTo(RGB(0,0, 255), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.blueLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

    //HSV fadeTo

  case 20:
    //fadeTo HSV RED, from cyan
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to Cyan
      Clyde.ambientLight.setColor(HSV(0.5,1,255));
      delay(500);
      Serial.println("20 fadeTo red from cyan HSV / update");
      Clyde.ambientLight.fadeTo(HSV(0,1,255), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.redLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

  case 21:
    //fadeTo HSV GREEN, from red
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to red
      Clyde.ambientLight.setColor(HSV(0,1,255));
      delay(500);
      Serial.println("21 fadeTo green from red HSV / update");
      Clyde.ambientLight.fadeTo(HSV(0.33,1,255), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.greenLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

  case 22:
    //fadeTo HSV BLUE, from green
    //on first pass through this test we request a fade
    if(!stayInCurrentTest){
      //Set color to green
      Clyde.ambientLight.setColor(HSV(0.33,1,255));
      delay(500);
      Serial.println("22 fadeTo blue from green hsv / update");
      Clyde.ambientLight.fadeTo(HSV(0.67,1,255), 0.01);
      stayInCurrentTest = true;
    }
    else{
      //on all subsequent passes, we perform the fade.
      Clyde.ambientLight.update();
    }
    //Once CLyde is On, we can move to the next test
    if(Clyde.ambientLight.blueLight.brightness>=255){
      stayInCurrentTest = false;
    }

    break;

  case 23:   
    //getHue, red
    Clyde.ambientLight.setColor(HSV(0,1,255));
    Serial.println("23 get hue, should be 0");
    Serial.println(Clyde.ambientLight.getHue());
    break;

  case 24:   
    //getHue, red
    Clyde.ambientLight.setColor(HSV(0.33,1,255));
    Serial.println("24 get hue, should be 0.33");
    Serial.println(Clyde.ambientLight.getHue());
    break;

  case 25:   
    //getHue, red
    Clyde.ambientLight.setColor(HSV(0.67,1,255));
    Serial.println("25 get hue, should be 0.67");
    Serial.println(Clyde.ambientLight.getHue());
    break;

  case 26:   
 
    //cycle through hue, red to blue
    if(!stayInCurrentTest){
      //Set color to red
      Clyde.ambientLight.setColor(HSV(hue,1,255));
      delay(500);
      Serial.println("Cycle through colors with hue");
      stayInCurrentTest = true;
    }
    else{
      Serial.println(hue);
      Clyde.ambientLight.setColor(HSV(hue,1,255));
      hue = hue + 0.001;
      delay(50);
    }
    //Once CLyde is On, we can move to the next test
    if(hue>=1){
      stayInCurrentTest = false;
      hue = 0.0;
    }
    break;

  default:
    testNumber = 0;
  }

  if(!stayInCurrentTest){
    delay(2000);
    testNumber++;
    Serial.println();
  }

}
