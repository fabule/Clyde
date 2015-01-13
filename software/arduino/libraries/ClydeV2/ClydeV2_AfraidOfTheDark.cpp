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

#include "ClydeV2_AfraidOfTheDark.h"

#ifdef ENABLE_AFRAID_OF_THE_DARK

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

ClydeV2_AfraidOfTheDark AfraidOfTheDark; // The Afraid of the Dark singleton.

bool ClydeV2_AfraidOfTheDark::init(uint8_t _apin, uint8_t _dpin) {

  #ifdef CLYDE_DEBUG_AFRAID
    Serial.println("AofD: init");
  #endif

  apin = _apin;
  dpin = _dpin;

  // Setup digital pin
  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);
  
  return true; 
}

void ClydeV2_AfraidOfTheDark::update(ClydeV2* c) {
  
  // Read the current light level
  readLightLevel();

  //  Afraid of the Dark triggers the SunsetBehavior if 
    //  The task light is off
    //  the behavior is not locked.  The lock is activated when 
    //  the light level is currently below the start threshold
    //  and it was above the start threshold the last time update ran.

  if(c->showPersonality){

    //  If the white light is on, then  block behavior by ignoring light levels for a duration. 
    if (c->taskLight.isOn()) {

      #ifdef CLYDE_DEBUG_AFRAID
        Serial.println("AofD: task light is on, lock");
      #endif

      lockDuration = millis() + ClydeV2_AfraidOfTheDark::WHITE_LIGHT_LOCK_TIME;
    }
    //  If the behavior is locked, then return
    else if (millis() < lockDuration) {

      #ifdef CLYDE_DEBUG_AFRAID
        Serial.println("AofD: locked");
      #endif

      return;
    }
    //  Check for decrease in light levels.  
      //  Detected if light level was above START_THRESHOLD last time update() ran, 
      //  now it is below.
    //  If the light level is below the threshold
    else if (currentLight <= ClydeV2_AfraidOfTheDark::START_THRESHOLD) {

      #ifdef CLYDE_DEBUG_AFRAID
        Serial.println("AofD: light level below threshold");
        Serial.print("lastLight ");
        Serial.println(lastLight);
        Serial.print("ready ");
        Serial.println(ready);
      #endif

      //  and the last time we checked it was above
      //  then set the time until which we need to remain below threshold
      if (lastLight > ClydeV2_AfraidOfTheDark::START_THRESHOLD) {
        lockDuration = millis() + ClydeV2_AfraidOfTheDark::THRESHOLD_LOCK_TIME;
      }
      //  If we're ready to trigger response to light levels decrease 
      else if (ready) {
          //  Decrease in the light levels detected.  Trigger response.
          lightDecreaseAction(c);
          ready = false;
      }
    }
    //Reset the ready flag
    //if the light is bright enough, aka above the reset threshold, then
    //reset the ready flag so that we can
    //start checking for the start of a new sunset
    //TODO: is this where we would add the sunrise???
    else if (!ready && currentLight >= ClydeV2_AfraidOfTheDark::RESET_THRESHOLD) {

      #ifdef CLYDE_DEBUG_AFRAID
        Serial.println("AofD: threshold reset.");
      #endif

      ready = true;
    }
  }

}

uint16_t ClydeV2_AfraidOfTheDark::readLightLevel() {

  lastLight = currentLight; //store the previous light level
  currentLight = analogRead(apin);  //read the current level

  #ifdef CLYDE_DEBUG_AFRAID
   // Serial.print("Clyde: last light level = ");
   // Serial.println(lastLight);
   // Serial.print("Clyde: current light level = ");
   // Serial.println(currentLight);
  #endif

  return currentLight;
}

/** Triggered when increasing light level is detected. ie Sunrise */
void ClydeV2_AfraidOfTheDark::lightIncreaseAction(ClydeV2* c){

  #ifdef CLYDE_DEBUG_AFRAID
    Serial.println("AofD: lightIncreaseAction");
  #endif

  // Start sunrise behavior
  c->startSunriseBehavior();
}

/** Triggered when decreasing light level is detected. ie Sunset */
void ClydeV2_AfraidOfTheDark::lightDecreaseAction(ClydeV2* c){

  #ifdef CLYDE_DEBUG_AFRAID
    Serial.println("AofD: lightDecreaseAction");
  #endif

  // Start sunset behavior
  c->startSunsetBehavior();
}

/** Triggered when flicking light levels detected.  ie. flashing the light switch. */
void ClydeV2_AfraidOfTheDark::lightFlickerAction(ClydeV2* c){

  #ifdef CLYDE_DEBUG_AFRAID
    Serial.println("AofD: lightFlickerAction");
  #endif

  // Start hypno behavior
  c->startHypnoBehavior();
}

#endif