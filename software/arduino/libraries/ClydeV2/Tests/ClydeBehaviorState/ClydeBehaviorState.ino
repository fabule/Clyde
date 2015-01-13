#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

//Test Behavior State

#define FIRMWARE_VERSION 2

void setup() {
  
  Wire.begin();
  Clyde.begin();

  Serial.begin(9600); // start serial for debugging output
  delay(2000);
  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Behavior State");
  Serial.println();
  
  //Clyde.ambientLight.setColor(RGB(255,255,255));
  //Clyde.ambientLight.fadeTo(RGB(0,255, 0), 5);
  //Clyde.ambientLight.startColorSelect(1);
  //Clyde.ambientLight.startBlink(RGB(255,0,0), 1000, 1000, 0);
  
  //Clyde.startBlinkBehavior(RGB(255,0,255),1000, 1000,0);
  //Clyde.startSelectBehavior();
  // Clyde.endBehavior();
  //Clyde.startLaughBehavior();
  // Clyde.endBehavior();
  Clyde.startSunsetBehavior();
  
  Serial.println("end of setup");
}

void loop() {
  
  Clyde.update();
  
}
