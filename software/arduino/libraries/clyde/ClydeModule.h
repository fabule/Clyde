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

#ifndef __CLYDEMODULE_H
#define __CLYDEMODULE_H

#include <stdint.h>

/**
 * Abstract Clyde module class.
 */
class CClydeModule {
private:
  const uint16_t ID_LOW;  // low limit of read identification value
  const uint16_t ID_HIGH; // high limit of read identification value
  
public:
  /**
   * Constructor.
   */
  CClydeModule(uint16_t low, uint16_t high) : ID_LOW(low), ID_HIGH(high) {};
  
  /**
   * Checks if the module matches a read identification value.
   * The value is read from a resistor attached to the module's analog pin,
   * which returns a value between 0 and 1023. Each module type is assigned
   * a narrow range.
   */
  bool id(uint16_t value) { return value >= ID_LOW && value <= ID_HIGH; }
  
  /**
   * Initialize the module with specified analog and digital pins.
   */
  virtual bool init(uint8_t apin, uint8_t dpin) = 0;
  
  /**
   * Update the module using the specified analog and digital pins.
   */ 
  virtual void update(uint8_t apin, uint8_t dpin) = 0;
};

#endif
