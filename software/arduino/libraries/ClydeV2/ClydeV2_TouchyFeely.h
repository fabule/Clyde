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

#ifndef __CLYDETOUCHYFEELY_H
#define __CLYDETOUCHYFEELY_H

#include "ClydeV2_Module.h"
#include "ClydeV2.h"
#include "MPR121.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/**
 * The Touchy Feely personality module.
 */
class ClydeV2;
class ClydeV2_TouchyFeely : public ClydeV2_Module {

  // -- resistor id values
  static const uint16_t ID_LOW = 1000;    // < low limit of read identification value
  static const uint16_t ID_HIGH = 1010;   // < high limit of read identification value

  // -- mpr121 related
  static const uint8_t DEVICE_ADDR = 0x5A;    // < i2c address of mpr121
  static const uint8_t TOUCH_LEVEL = 0x06;    // < touch threshold of mpr121
  static const uint8_t RELEASE_LEVEL = 0x04;  // < release threshold of mpr121

  // -- tickle control
  static const uint32_t TICKLE_INTERVAL = 2500;   // < max time in millis between touch events to trigger laugh
  static const uint8_t TICKLE_REPEAT = 4;         // < number of consecutive touch events to trigger laugh
  
  // -- touch control
  static const uint8_t TOUCH_CONFIRM_DURATION = 250;   // < delay from first detected touch to classifying as a touch, if any leg is touched
  static const uint8_t LEG_START_THRESH = 100;    // < delay from first detected touch to classifying as a touch, for a particular leg
  static const uint8_t LEG_STOP_THRESH = 1000;    // < delay from the last detected touch to the detected touch time, for classifying as released
    
  
  static const uint8_t NUM_LEGS = 6;    // < just in case there is one day a mega-clyde with 5 billion legs
  static const uint8_t DAISY_LEGS = 3;

  MPR121 mpr121;           // < interface to the mpr121
  uint8_t numLegsTouched;  // < number of legs currently touched
  uint8_t tickleCount;     // < number of detected tickle touch events
  uint32_t firstTickle;    // < time in millis we detected the first tickle tap
  uint16_t touchStatus;    // < current status of touch electrodes.
  uint32_t touchStart;     // < time in millis when the active touch started.
  bool lastAmbientOn;      // < status of the ambient light on last update.
  bool lastWhiteOn;        // < status of the white light on last update.
  
private:

  bool legTouched[NUM_LEGS];
  long legStartTouchTime[NUM_LEGS];

public:

  /** Constructor. */
  ClydeV2_TouchyFeely()
    : ClydeV2_Module(ClydeV2_TouchyFeely::ID_LOW, ClydeV2_TouchyFeely::ID_HIGH), 
    mpr121(ClydeV2_TouchyFeely::DEVICE_ADDR, ClydeV2_TouchyFeely::TOUCH_LEVEL, ClydeV2_TouchyFeely::RELEASE_LEVEL),
    touchedHandler(NULL),
    releasedHandler(NULL),
    touchStart(0),
    numLegsTouched(0),
    tickleCount(0),
    firstTickle(0),
    lastAmbientOn(false),
    lastWhiteOn(false) {}
  
  /**  Initialize the module with specified analog and digital pins. */  
  bool init(uint8_t apin, uint8_t dpin);

  /**  Update the module using the specified analog and digital pins. */  
  void update(ClydeV2* c);
  /**  Update individual leg touches. */
  void updateLegs(ClydeV2* c);
  /**  Process the leg touch status for a particular leg. */
  void processLegTouchStatus(int legNum, bool legTouchStatus);

  /**  Set the touched handler.  Triggered by any leg touch, after touch confirmation */
  void setTouchedHandler(void(*function)()) { touchedHandler = function; }
  /**  Set the released handler. Triggered by any leg release. */
  void setReleasedHandler(void(*function)()) { releasedHandler = function; }
  /**  Set the touched handler.  Triggered by specific leg touch initial detection. */
  void setLegDetectedHandler(void(*function)(uint8_t _leg)) { legDetectedHandler = function; }
  /**  Set the touched handler.  Triggered by specific leg touch confirmation. */
  void setLegTouchedHandler(void(*function)(uint8_t _leg)) { legTouchedHandler = function; }
  /**  Set the released handler.  Triggered by specific leg release. */
  void setLegReleasedHandler(void(*function)(uint8_t _leg)) { legReleasedHandler = function; }

  // -- callback methods
  void (*touchedHandler)();
  void (*releasedHandler)();
  void (*legTouchedHandler)(uint8_t l);
  void (*legReleasedHandler)(uint8_t l);
  void (*legDetectedHandler)(uint8_t l);

private:

  /** Check if the detected touch is tickling. */
  void tickleCheck(ClydeV2* c);
  
  /** Start laughing. */
  void laughAction(ClydeV2* c);
  /** Start daisy behavior. */
  void daisyAction(ClydeV2* c);
  /** Start the touch reaction. ie. color select behavior. */
  void startTouchAction(ClydeV2* c);
  /** Stop the touch reaction. ie. color select behavior. */
  void stopTouchAction(ClydeV2* c);
  
  /**  Reset the MPR121 when light status changes. */
  void resetMPR121(ClydeV2* c);
 
  /** Debug autoconfig values. */
  //void debugAutoConfig();

};

//TODO
//using this as a singleton means that it would get
//confused if we're using two identical modules
extern ClydeV2_TouchyFeely TouchyFeely;

#endif