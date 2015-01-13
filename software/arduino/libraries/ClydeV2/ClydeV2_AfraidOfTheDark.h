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

#ifndef __CLYDEAFRAIDOFTHEDARK_H
#define __CLYDEAFRAIDOFTHEDARK_H

#include "ClydeV2_Module.h"
#include "ClydeV2.h"

/**
 * The Afraid of the Dark personality module.
 */
class ClydeV2;
class ClydeV2_AfraidOfTheDark : public ClydeV2_Module {

  // -- resistor id vals
  static const uint16_t ID_LOW = 900;                 // < low limit of read identification value
  static const uint16_t ID_HIGH = 920;                // < high limit of read identification value
  // -- trigger thresholds
  static const uint16_t START_THRESHOLD = 20;         // < threshold to reach below to start the sunset cycle
  static const uint16_t RESET_THRESHOLD = 50;         // < threshold to reach above to start checking to starting a new sunset cycle
  // -- behavior locks
  static const uint16_t THRESHOLD_LOCK_TIME = 1000;   // < time in millis to stay beyond threshold to trigger
  static const uint16_t WHITE_LIGHT_LOCK_TIME = 2000; // < time in millis to lock after white light is on
  
  uint32_t lockDuration;  // < time in millis until when we need to stay below threshold to trigger sunset
  uint16_t currentLight;  // < the current light level
  uint16_t lastLight;     // < last detected light level
  bool ready;             // < true if we are checking for low light level trigger 
  
public:  

  /** Constructor. */
  ClydeV2_AfraidOfTheDark()
    : ClydeV2_Module(ClydeV2_AfraidOfTheDark::ID_LOW, ClydeV2_AfraidOfTheDark::ID_HIGH),
    lockDuration(0),
    currentLight(0),
    lastLight(0),
    ready(false) {}

  /** Initialize the module with specified analog and digital pins. */
  bool init(uint8_t _apin, uint8_t _dpin);
  
  /** Update the module using the specified analog and digital pins. */   
  void update(ClydeV2* c);

  /** Read the ambient light sensor, and return the value. */
  uint16_t readLightLevel();

private:

  /** Action called when increasing light level is detected. ie Sunrise */
  void lightIncreaseAction(ClydeV2* c);
  /** Action called when decreasing light level is detected. ie Sunset */
  void lightDecreaseAction(ClydeV2* c);
  /** Action called when flicking light levels detected.  ie. flashing the light switch. */
  void lightFlickerAction(ClydeV2* c);

};

//TODO
//using this as a singleton means that it would get
//confused if we're using two identical modules
extern ClydeV2_AfraidOfTheDark AfraidOfTheDark;

#endif