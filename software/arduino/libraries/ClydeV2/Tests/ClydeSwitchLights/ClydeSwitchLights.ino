#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>

//Test Switch Lights

#define FIRMWARE_VERSION 2

int testNumber = 0;

void setup() {
  Wire.begin();
  Clyde.begin();

  Serial.begin(9600); // start serial for debugging output
  delay(2000);

  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Test Switch Lights");
  Serial.println();
}

void loop() {
 
  Clyde.update();
  
  if(testNumber==25000){
    //Clyde.switchLights();
    Clyde.onPress();
    testNumber = 0;
  }
  
  testNumber++;
}
