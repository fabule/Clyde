#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SPI.h>
#include <MPR121.h>

#define FIRMWARE_VERSION 2

/*
 * Clyde Test Afraid of the Dark
 */

void setup() {
  Wire.begin();
  Clyde.begin();
  Serial.begin(9600);
  delay(4000);

  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Test Afraid of the Dark");
  Serial.println();

}

void loop() {
  Clyde.update();
  delay(1000);
}

