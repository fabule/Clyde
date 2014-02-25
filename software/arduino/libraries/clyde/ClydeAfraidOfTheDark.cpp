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

#include "ClydeAfraidOfTheDark.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

CClydeAfraidOfTheDark AfraidOfTheDark;

CClydeAfraidOfTheDark::CClydeAfraidOfTheDark() : CClydeModule(ID_LOW, ID_HIGH) {
  m_lock = 0;
  m_lastLight = 0;
  m_ready = false;
  m_sunsetSteps = 0;
}

bool CClydeAfraidOfTheDark::init(uint8_t apin, uint8_t dpin) {
  //read cycle from eeprom
  if (!Clyde.eeprom()->readSunsetCycle(&m_sunsetSteps, &m_sunsetColors[0], &m_sunsetIntervals[0])) {
    #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Failed to initialize Afraid of the Dark personality. Error reading sunset cycle from EEPROM.");
    #endif
    return false;
  }

  //setup pins
  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);
  
  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: Afraid of the Dark personality initialized.");
  #endif
  
  return true; 
}

void CClydeAfraidOfTheDark::update(uint8_t apin, uint8_t dpin) {
  unsigned short light = analogRead(apin);  

  //if the light level is below the threshold
  if (!Clyde.white()->isOn() && light <= START_THRESHOLD) {
    //and the last time we checked it was above
    //then set the time until which we need to remain below threshold
    if (m_lastLight > START_THRESHOLD) {
        m_lock = millis() + THRESHOLD_LOCK_TIME;
    }
    //if we're ready for a sunset, and we reached the set time, go sunset
    else if (m_ready && millis() > m_lock) {
        startSunset();
        m_ready = false;
    }
  }
  //if the light is above the reset threshold, then
  //start checking for the start of a new sunset
  else if (!m_ready && light >= RESET_THRESHOLD) {
    m_ready = true;
    #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Afraid of the Dark personality threshold reset.");
    #endif
  }

  m_lastLight = light;
}

//TODO probably doesn't need extra method
void CClydeAfraidOfTheDark::startSunset() {
  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: Afraid of the Dark personality start sunset.");
  #endif

  if (Clyde.cycle()->is(SUNSET))
    return;
  
  Clyde.setCycle(SUNSET, m_sunsetSteps, m_sunsetColors, m_sunsetIntervals, NO_LOOP);
}