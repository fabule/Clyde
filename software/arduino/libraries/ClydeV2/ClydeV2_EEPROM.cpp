/*
  Copyright (c) 2013-2014, Fabule Fabrications Inc, All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, version 3.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library.
*/

#include "ClydeV2_EEPROM.h"
#include "ClydeV2.h"
  
#include "EEPROM.h"
#include <string.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CLYDE_SERIAL_LENGTH 6

void ClydeV2_EEPROM::writeSerial(char *serial) {
  for(int i = 0; i < CLYDE_SERIAL_LENGTH; i++)
    //verify hexadecimal string
    //TODO replace with 'characters', clearer
    if (*(serial + i) < 48 || *(serial + i) > 70 ||
        (*(serial + i) > 59 && *(serial + i) < 65))
      return;
  
  for(int i = 0; i < CLYDE_SERIAL_LENGTH; i++)
    EEPROM.write(i, *(serial+i));
}

void ClydeV2_EEPROM::readSerial(char* serial) {
  for(int i = 0; i < CLYDE_SERIAL_LENGTH; i++)
    *(serial + i) = EEPROM.read(i);
}

void ClydeV2_EEPROM::writeQC(bool qc) {
  EEPROM.write(QC_ADDR, qc ? 1 : 0);
}
  
void ClydeV2_EEPROM::readQC(bool* qc) {
  *(qc) = EEPROM.read(QC_ADDR) == 1;
}

void ClydeV2_EEPROM::writeAmbientColor(RGB* color) {
  EEPROM.write(AMBIENT_ADDR + 0, color->r);
  EEPROM.write(AMBIENT_ADDR + 1, color->g);
  EEPROM.write(AMBIENT_ADDR + 2, color->b);
}

void ClydeV2_EEPROM::readAmbientColor(RGB* color) {
  *color = RGB(
    EEPROM.read(AMBIENT_ADDR + 0),
    EEPROM.read(AMBIENT_ADDR + 1),
    EEPROM.read(AMBIENT_ADDR + 2)
  );
}

void ClydeV2_EEPROM::reset() {
  //char serial[7] = "012345";
  uint16_t ver = 1;
  RGB ambient(20,255,54); //Default Clyde Color - Cyan/Teal
  
  //writeSerial(&serial[0]);
  //writeQC(false);
  writeAmbientColor(&ambient);
}