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
#ifndef __CLYDE_COLORCYCLE_H
#define __CLYDE_COLORCYCLE_H

#include "ClydeV2_ColorTypes.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/**
 * Enum types for cycle loop options.
 */
enum ECycleLoop {
  LOOP,
  LOOP_ONCE,
  NO_LOOP
};

/**
 * The Color Cycle
 */
class ClydeV2_ColorLight;
class ClydeV2_ColorCycle {

public:
  
    static const uint8_t MAX_CYCLE_LENGTH = 12; // < Maximum cycle steps.
    bool isCycling;

private:

  ClydeV2_ColorLight* colorLight;                     // < The color light controlled by this cycle.
  
  uint8_t numSteps;                           // < Number of steps.
  uint8_t step;                               // < Current step.
  uint32_t stepStart;                         // < Time when the current step started in millis.
  uint32_t stepEnd;                           // < Time when the current step end in millis.
  RGB stepColor;                              // < Start color of the current step.
  RGB colors[ClydeV2_ColorCycle::MAX_CYCLE_LENGTH];   // < Colors of cycle.
  uint32_t intervals[ClydeV2_ColorCycle::MAX_CYCLE_LENGTH];   // < Intervals in millis.
  ECycleLoop loop;                            // < Looping parameter.

public:

  /** Constructor. */
  ClydeV2_ColorCycle();
  
  /**  Initialize */  
  bool init(ClydeV2_ColorLight* colorLight);
  /**  Update the color cycle.  Handles color fading. */
  void update();
  /**  Check if there is an ongoing cycle. */
  bool isOn();
  /** Get the duration of the cycle in millseconds */
  uint32_t getDuration();

  /**  Set the ambient light cycle. */
  void setCycle(uint8_t steps, const RGB *colors, const uint8_t *intervals, ECycleLoop loop);
  void setCycle(uint8_t steps, const RGB *colors, const uint16_t *intervals, ECycleLoop loop);
  void setCycle(uint8_t steps, const RGB *colors, const uint32_t *intervals, ECycleLoop loop);
  
  /**  Set the cycle step. */
  void setCycleStep(uint8_t step);
  /**  Get the cycle step. */
  uint8_t getCycleStep( );
  /**  Next step. */
  void nextStep(uint32_t now);

  /**  Speed up cycle. */
  void speedUp(uint32_t factor);
  /**  Stop the cycle. */
  void stop();

private:

  /**  Set the ambient light cycle. */
  void setCycle(uint8_t steps, const RGB* colors, ECycleLoop loop);
  /**  Update the ambient light cycle to the next step. */
  void updateNextStep(uint32_t now);
  
};

#endif