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

#ifndef __CLYDE_H
#define __CLYDE_H

#include <stdint.h>
#include "ClydeV2_Config.h"
#include "ClydeV2_DebugFlags.h"
#include "ClydeV2_SquishyEye.h"
#include "ClydeV2_Light.h"
#include "ClydeV2_ColorLight.h"
#include "ClydeV2_ColorTypes.h"
#include "ClydeV2_EEPROM.h"
#include "ClydeV2_BehaviorState.h"
#include "ClydeV2_Module.h"
#include "ClydeV2_ModuleDock.h"

#ifdef ENABLE_AFRAID_OF_THE_DARK
  #include "ClydeV2_AfraidOfTheDark.h"
#endif 

#ifdef ENABLE_TOUCHY_FEELY
  #include "ClydeV2_TouchyFeely.h"
#endif

#ifdef ENABLE_CHATTERBOX
  #include "ClydeV2_Chatterbox.h"
#endif

/**
 * Main Clyde class that provides the interface to the device.
 */
class ClydeV2_BehaviorState;
class ClydeV2 {
public:

  ClydeV2_Light taskLight;
  ClydeV2_ColorLight ambientLight;
  ClydeV2_SquishyEye squishyEye;
  ClydeV2_EEPROM eeprom;
  
  bool enableSquishyEyeButton;
  bool showPersonality;

  #ifdef ENABLE_CHATTERBOX
    ClydeV2_Chatterbox chatterbox;
  #endif

private:
  ClydeV2_ModuleDock moduleDocks[ClydeV2_ModuleDock::NUM_MODULES];
  ClydeV2_BehaviorState* _behavior;

  friend class ClydeV2_SquishyEye;
  friend class ClydeV2_Module;
  friend class ClydeV2_AfraidOfTheDark;
  friend class ClydeV2_TouchyFeely;

  friend class ClydeV2_BehaviorState;
  void changeBehavior(ClydeV2_BehaviorState*);

public:
  /** Contructor. */
  ClydeV2();
  /** Initialize Clyde. */
  void begin();
  /** Show any error messages from initilization */
  bool showInitErrors();
  /** Update Clyde's state */
  void update();
  /** Each time this function is called, Clyde switchs to a new light state, ie: ambient light on,both on,task light on, off. */
  void switchLights();

  void onPress();

  bool startBlinkBehavior(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks);
  bool startSelectBehavior();
  bool startLaughBehavior();
  bool startDaisyBehavior();
  bool startSunsetBehavior();
  bool startSunriseBehavior();
  bool startHypnoBehavior();

  void endBehavior();

private:

  /** Detect the personality modules. */
  void detectPersonalities();

  void updatePersonalities();
  
};

extern ClydeV2 Clyde; 

#endif
