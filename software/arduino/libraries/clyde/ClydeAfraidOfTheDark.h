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

#include "ClydeModule.h"
#include "Clyde.h"

/**
 * The Afraid of the Dark personality module.
 */
class CClydeAfraidOfTheDark : public CClydeModule {
  static const uint16_t ID_LOW = 900;                 /**< low limit of read identification value */
  static const uint16_t ID_HIGH = 920;                /**< high limit of read identification value */
  static const uint16_t START_THRESHOLD = 20;         /**< threshold to reach below to start the sunset cycle */
  static const uint16_t RESET_THRESHOLD = 50;         /**< threshold to reach above to start checking to starting a new sunset cycle */
  static const uint16_t THRESHOLD_LOCK_TIME = 1000;    /**< time in millis to stay beyond threshold to trigger */
  static const uint16_t WHITE_LIGHT_LOCK_TIME = 2000; /**< time in millis to lock after white light is on */
  
  uint32_t m_lock;        /**< time in millis until when we need to stay below threshold to trigger sunset */
  uint16_t m_lastLight;   /**< last detected light level */
  bool m_ready;           /**< true if we are checking for low light level trigger */
  RGB m_sunsetColors[CClyde::CAmbientCycle::MAX_CYCLE_LENGTH];         /**< color of the sunset cycle */
  uint32_t m_sunsetIntervals[CClyde::CAmbientCycle::MAX_CYCLE_LENGTH];  /**< intervals of the sunset cycle */
  uint8_t m_sunsetSteps;  /**< number of steps in the sunset cycle */
  
public:  
  /** Constructor. */
  CClydeAfraidOfTheDark();
  
  /**
   * Initialize the module with specified analog and digital pins.
   */
  bool init(uint8_t apin, uint8_t dpin);
  
  /**
   * Update the module using the specified analog and digital pins.
   */   
  void update(uint8_t apin, uint8_t dpin);
  
private:
  /** Start the sunset cycle. */
  void startSunset();
};

//TODO
//using this as a singleton means that it would get
//confused if we're using two identical modules
extern CClydeAfraidOfTheDark AfraidOfTheDark;

#endif