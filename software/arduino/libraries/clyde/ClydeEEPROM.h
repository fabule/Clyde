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
  6-7:    firmware version (e.g. 1)
  8-10:   start ambient color (e.g. 255, 255, 255)
  11:     # of sunset steps (e.g. 6)
  12-47:  sunset colors (e.g. R,G,B,R,G,B... max = 12)
  48-95:  32-bit sunset color intervals (e.g. millis, millis... max = 12)
*/

#ifndef __CLYDEEEPROM_H
#define __CLYDEEEPROM_H

#include <stdint.h>

#include "colortypes.h"

/**
 * Utility class to manage Clyde's EEPROM.
 */
class CClydeEEPROM {
  static const uint16_t VERSION_ADDR = 6;                    //address of the firmware version
  static const uint16_t AMBIENT_ADDR = VERSION_ADDR+2;       //address of the start ambient color
  static const uint16_t SUNSET_ADDR = AMBIENT_ADDR+3;        //address of the sunset cycle block address

public:
  void writeSerial(char *serial);
  void readSerial(char *serial);

  void writeVersion(uint16_t vers);    
  void readVersion(uint16_t *vers);

  void writeAmbientColor(RGB *color);
  void readAmbientColor(RGB *color);

  bool clearSunsetCycle();
  bool writeSunsetCycle(uint8_t steps, RGB *colors, uint32_t *intervals);
  bool readSunsetCycle(uint8_t *steps, RGB *colors, uint32_t *intervals);
  
  void reset();
};

#endif