#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>


#define FIRMWARE_VERSION 2

void setup() {
  Wire.begin();
  Clyde.begin();

  //Clyde.squishyEye.setPressedHandler(eyePressed);
  Clyde.squishyEye.setReleasedHandler(eyeReleased);

  delay(2000);

  Serial.println("Clyde is Ready!");
  Clyde.taskLight.turnOn();
  Clyde.ambientLight.turnOn();
  
   Clyde.onPress();
   Clyde.onPress();
}

void loop() {
  
 
  Clyde.update();

  /*
  Clyde.onPress();
  Clyde.update();
  delay(1000);
  
  Clyde.onPress();
  Clyde.update();
  delay(1000);
  
  Clyde.onPress();
  Clyde.update();
  delay(1000);
  */
  //Serial.println(".");
  //Clyde.squishyEye.update();
  //Clyde.ambientLight.update();
  //Clyde.taskLight.update();

  /*
  //if the eye was pressed
   if (Clyde.squishyEye.wasPressed()) {
   Serial.println("Clyde: eye was pressed.");
   // delay(5000);
   switchLights();
   
   //and the ambient cycle is on, then turn it off
   if (isOn()) 
   stopCycle();
   //if not, then switch the lights to the next state
   else
   switchLights();
   }
   */

}


void eyePressed() {
  Serial.println("EYE PRESSED CALLBACK");
  return;
}

void eyeReleased() { 
  Serial.println("EYE RELEASED CALLBACK");
   Clyde.onPress();
  //switchLights();
  /*
   //and the ambient cycle is on, then turn it off
   if (isOn()) 
   stopCycle();
   //if not, then switch the lights to the next state
   else
   switchLights();
   */
   
   return;
}



