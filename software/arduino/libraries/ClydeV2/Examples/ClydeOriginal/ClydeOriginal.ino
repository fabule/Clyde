#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <MPR121.h>
#include <ClydeV2.h>

#define FIRMWARE_VERSION 2

void setup() {
  Wire.begin();
  Clyde.begin();
}

void loop() {
  Clyde.update();
}
