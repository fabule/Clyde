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

/*
  EEPROM map
  0-5:    serial number (e.g. FFFFFF)
  6:      QC passed (e.g. 1)
  7-9:   start ambient color (e.g. 255, 255, 255)
  10:     # of sunset steps (e.g. 6)
  11-46:  sunset colors (e.g. R,G,B,R,G,B... max = 12)
  47-94:  32-bit sunset color intervals (e.g. millis, millis... max = 12)
*/

#ifndef __CLYDEEEPROM_H
#define __CLYDEEEPROM_H

#include <stdint.h>

#include "colortypes.h"

/**
 * Utility class to manage Clyde's EEPROM.
 */
class CClydeEEPROM {
  static const uint16_t QC_ADDR = 6;                         //address of the QC passed bit
  static const uint16_t AMBIENT_ADDR = QC_ADDR+1;            //address of the start ambient color
  static const uint16_t SUNSET_ADDR = AMBIENT_ADDR+3;        //address of the sunset cycle block address

public:
  void writeSerial(char *serial);
  void readSerial(char *serial);

  void writeQC(bool qc);    
  void readQC(bool *qc);

  void writeAmbientColor(RGB *color);
  void readAmbientColor(RGB *color);

  bool clearSunsetCycle();
  bool writeSunsetCycle(uint8_t steps, RGB *colors, uint32_t *intervals);
  bool readSunsetCycle(uint8_t *steps, RGB *colors, uint32_t *intervals);
  
  void reset();
};

#endif