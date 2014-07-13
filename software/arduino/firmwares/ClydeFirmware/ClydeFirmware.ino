/*
  Copyright (c) 2013-2014, Fabule Fabrications Inc, All rights reserved.
 
 This is the main Clyde firmware that controls the user interface.
 
 
 Function mapping and descriptions by N. Seidle July 12, 2014
 
 Clyde.updateEye: Calibrate the eye and check for press
 Clyde.updateMouth: update the mouth to play sounds
 
 Clyde.setAmbient(RGB(r, g, b)): Sets the ambient color of Clyde's eye without crossfade. 0 is off.
 Clyde.setWhite(w): Sets the white level without crossfade (do it now). 0 is full blast, 250 is super soft. 255 is off.
 
 Clyde.fadeAmbient(RGB(r, g, b), speed): Set a color to go to over time. Speed is a float. 1.0 is very fast. 0.03 is nice and slow.
 
 Clyde.updateAmbientLight: update the lights
 Clyde.updateWhiteLight: Slowly fade from the .targetBrightness to .brightness. Do so at the .fadeSpeed.
 
 Clyde.wasEyePressed: returns 
 
 //make Clyde behave after the eye was calibrated once
 if (Clyde.wasEyeCalibratedOnce())
 
 New behavior:
 RGB slowly moves through different colors.
 
 When I press the button, light comes on. Press again, light off.
 Press and hold, the lamp gets more bright until it wraps to very dim.
 
 */

#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h> //Library to tokenize and parse space separated commands receieved over serial
#include <SoftwareSerial.h> //Library to give any pin the ability to send or receive serial
#include <MPR121.h> //Library handles the capacitive touch input

#define FIRMWARE_VERSION 1

SerialCommand sCmd;

int currentLevel = 255; //Used to keep track of the brightness of the desk light. 255 is off. 0 is 100% on.

long lastChange = 0; //Used to track of the last time we had a color change

void setup() {

  Clyde.begin(); //Setup all the analog inputs and output pins.

  //Fade white reading light from bright to off
  Clyde.setWhite(0); //analogWrite(m_white.pin, 0);
  delay(500);
  for(int x = 0 ; x < 256 ; x++)
  {
    Clyde.setWhite(x);
    delay(4);
  }

  randomSeed(analogRead(A5)); //Seed the random generator with noise

  Wire.begin(); //I2C being, used for cap sense

  Serial.begin(9600);

  //?: Create incoming commands that will be recognized by user's sending serial to Clyde
  sCmd.addCommand("SERIAL", cmdSerial);
  sCmd.addCommand("VERSION", cmdVersion);
  sCmd.addCommand("RESET", cmdReset);
  sCmd.addCommand("SET_AMBIENT", cmdSetAmbient);
  sCmd.addCommand("SET_WHITE", cmdSetWhite);
  sCmd.addCommand("WRITE_EEPROM", cmdWriteEEPROM);
  sCmd.addCommand("READ_EEPROM", cmdReadEEPROM);

  //Clyde.eeprom()->reset();

  Clyde.updateEye(); //Calibrate the eye

}

void loop() {
  //sCmd.readSerial(); //read the serial communication if any

  //Check to see if eye is pressed
  if(Clyde.checkEye())
  {
    //Toggle white light on each press
    if(currentLevel == 255)
    {
      //Turn on
      for(int x = 0 ; x < 256 ; x++)
      {
        Clyde.setWhite(255 - x);
        delay(4);
      }

      currentLevel = 0; //Remember that we are now on
    }
    else
    {
      //Turn off
      for(int x = 0 ; x < 256 ; x++)
      {
        Clyde.setWhite(x);
        delay(4);
      }

      currentLevel = 255; //Remember that we are now off
    }
  }

  /*if(Clyde.pressedLength() > 1000)
   {
   Serial.println("Long press");
   }*/

  //Change eye color every few seconds
  if(millis() - lastChange > 5000)
  {
    int r = random(0, 256); //0 to 255
    int g = random(0, 256); //0 to 255
    int b = random(0, 256); //0 to 255

    //Push more values to off, 25% chance
    if(random(100) < 25) r = 0; //Turn red off
    if(random(100) < 25) g = 0; //Turn green off
    if(random(100) < 25) b = 0; //Turn blue off

    Clyde.fadeAmbient(RGB(r, g, b), 0.02); //Sets the ambient color of Clyde's eye without crossfade. 0 is off.

    lastChange = millis();
  }

  Clyde.updateAmbientLight(); //Fade to a new color if there is one to go to
}

//
// Serial Commands
//

//Respond with the serial number associated with Clyde?
void cmdSerial() {
  char serial[7] = {
    0    };
  Clyde.eeprom()->readSerial(&serial[0]);  
  Serial.print("OK "); 
  Serial.println(serial);
}

void cmdVersion() {
  uint16_t vers = FIRMWARE_VERSION;
  Serial.print("OK "); 
  Serial.println(vers);
}

//?: Erases all of Clyde's settings
void cmdReset() {
  Clyde.eeprom()->reset();
  Serial.println("OK"); 
}

//Use the three incoming characters (RGB) and set Clydes ambient light to these values
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

//Receives an address and a value over serial
//Then records those values to EEPROM
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

//Receives an address over serial
//Reports the EEPROM value at that address
void cmdReadEEPROM() {
  char *param1;
  int addr;

  //Get arguments
  param1 = sCmd.next();    // Address
  addr = atoi(param1);  

  Serial.print("OK ");
  Serial.println(EEPROM.read(addr));
}


