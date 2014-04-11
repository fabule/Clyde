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

#include "ClydeTouchyFeely.h"
#include "I2Cdev.h"

#ifdef ENABLE_TOUCHY_FEELY

CClydeTouchyFeely TouchyFeely;

const RGB CClydeTouchyFeely::SELECT_COLORS[] = {RGB(255,0,0), RGB(255,255,0), RGB(0,255,0), RGB(0,255,255), RGB(0,0,255), RGB(255,0,255), RGB(255,255,255)};
const uint16_t CClydeTouchyFeely::SELECT_INTERVALS[] = {1000, 1000, 1000, 1000, 1000, 1000, 1000};
const uint8_t CClydeTouchyFeely::SELECT_STEPS = 7;
  
CClydeTouchyFeely::CClydeTouchyFeely()
  : CClydeModule(ID_LOW, ID_HIGH), m_mpr121(DEVICE_ADDR) {
  
  m_touchedHandler = NULL;
  m_releasedHandler = NULL;
  m_touchStart = 0;
  m_lastStopStep = 0;
  m_colorSelectEnabled = true;
  m_tickleCount = 0;
  m_firstTickle = 0;
}

bool CClydeTouchyFeely::init(uint8_t apin, uint8_t dpin) {
  if (!m_mpr121.testConnection()) {
    #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Failed to initialize Touchy-Feely personality. Failed to connect to MPR121.");
    #endif
    return false;
  }
  
  m_mpr121.initialize(false);
 
  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);
  
  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: Touchy-Feely personality initialized.");
  #endif
  
  return true;
}

void CClydeTouchyFeely::update(uint8_t apin, uint8_t dpin) {
  //only active when the ambient light is on
  if (m_colorSelectEnabled && !Clyde.ambient()->isOn()) return;

  //trigger touch event after a few millis to protect from false positive
  if ((m_touchStatus & 0x0FFF) && (millis()-m_touchStart > 25)) {
    //start color selection only if current cycle isn't laugh or select
    if (!Clyde.cycle()->is(SELECT) && !Clyde.cycle()->is(LAUGH))
      startColorSelect();
    
    //call touched handler if any
    if (m_touchedHandler) m_touchedHandler();
    
    //reset status to only call this once
    m_touchStatus = 0;
  }

  //check for mpr121 interrupt
  if (digitalRead(dpin))
    return;
  
  //read the touch state from the MPR121
  m_touchStatus = m_mpr121.getTouchStatus();

  //if clyde is not laughing already
  if (!Clyde.cycle()->is(LAUGH)) { //TODO better method names: Clyde.isLaughing()
    //and any leg is touched, then start color selection
    if (m_touchStatus & 0x0FFF) {
      #ifdef CLYDE_DEBUG
      Serial.println("Clyde: Touchy-Feely detected a touch.");
      #endif
      
      m_touchStart = millis();
    }
    //if leg is not touched, stop cycle
    else {
      #ifdef CLYDE_DEBUG
      Serial.println("Clyde: Touchy-Feely detected a release.");
      #endif
      
      stopColorSelect();  
      tickleCheck();
    }
  }
  //if clyde is laughing, check to tickles to see if it needs to continue
  else if (!(m_touchStatus & 0x0FFF)) {
    tickleCheck();
  }
  
  //call released handler if it is set and no legs are touched
  if (m_releasedHandler && !(m_touchStatus & 0x0FFF))
    m_releasedHandler();
}

void CClydeTouchyFeely::tickleCheck() {
  //touch detected, increase the tickle count
  m_tickleCount++;
  if (m_tickleCount == 1) {
    m_firstTickle = millis(); //keep track of the first tickle
  }
  //if the tickle is fast enough, close together
  else if (millis() < m_firstTickle + TICKLE_INTERVAL) {
    //and enough taps, then start laughing
    if (m_tickleCount >= TICKLE_REPEAT) {
      laugh();
      m_firstTickle = 0;
    }
    #ifdef CLYDE_DEBUG
    else {
      Serial.print("Clyde: touchy-feely module detected ");
      Serial.print(m_tickleCount);
      Serial.println(" tickle(s)");
    }
    #endif
  }
  //if it's been too long, reset tickle count
  else {
    m_tickleCount = 0;
  }
}

void CClydeTouchyFeely::laugh() {
  //we need at least three cycle steps to store tickle
  if (CClyde::CAmbientCycle::MAX_CYCLE_LENGTH < 2) return;

  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: laughs");
  #endif
  
  //generate random tickle cycle
  //TODO move values somewhere easier to find and change
  uint8_t laughSteps = random(CClyde::CAmbientCycle::MAX_CYCLE_LENGTH/4,
                              CClyde::CAmbientCycle::MAX_CYCLE_LENGTH/2) * 2;
  uint8_t i;
  for(i = 0; i < laughSteps; i+=2) {
    m_laughColors[i] = RGB(random(60, 100), 0, random(200, 255));
    m_laughIntervals[i] = random(100, 200);
    
    m_laughColors[i+1] = RGB(random(10, 40), 0, random(45, 63) * m_laughColors[i+1].r / 10);
    m_laughIntervals[i+1] = random(50, 100);
  }
  
  laughSteps += random(0, 2) * 2;
  for(; i < laughSteps; i+=2) {
    m_laughColors[i] = RGB(random(180, 255), random(80, 130), 0);
    m_laughIntervals[i] = random(300, 350);
    
    m_laughColors[i+1] = RGB(random(10, 40), 0, random(45, 63) * m_laughColors[i+1].r / 10);
    m_laughIntervals[i+1] = random(150, 200);
  }
  
  if (laughSteps < CClyde::CAmbientCycle::MAX_CYCLE_LENGTH)
    laughSteps++;
    
  m_laughColors[laughSteps-1] = RGB(Clyde.ambient()->color.r, Clyde.ambient()->color.g, Clyde.ambient()->color.b);
  m_laughIntervals[laughSteps-1] = random(150, 200);
  
  Clyde.setCycle(LAUGH, laughSteps, &m_laughColors[0], m_laughIntervals, NO_LOOP);
  Clyde.setPlayMode(PLAYMODE_SINGLE_CYCLE);
  Clyde.play(SND_LAUGH);
}

void CClydeTouchyFeely::startColorSelect() {
  if (!m_colorSelectEnabled) return;

  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: touchy-feely color selection cycle STARTED");
  #endif
  
  Clyde.setCycle(SELECT, SELECT_STEPS, SELECT_COLORS, SELECT_INTERVALS, LOOP);
  Clyde.setCycleStep(m_lastStopStep);
  
  Clyde.setPlayMode(PLAYMODE_SINGLE_CYCLE);
  Clyde.play(SND_HAPPY);
}

void CClydeTouchyFeely::stopColorSelect() {
  if (!m_colorSelectEnabled) return;

  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: touchy-feely color selection cycle STOPPED");
  #endif
  
  //save step to restart at the same place
  m_lastStopStep = Clyde.cycle()->step;
  
  Clyde.cycle()->off();
  Clyde.ambient()->save();
  
  //stop audio
  Clyde.stop();
}

/*
void CClydeTouchyFeely::debugAutoConfig() {
  //OOR
  uint8_t buf;
  I2Cdev::readByte(DEVICE_ADDR, ELE0_7_OOR_STATUS, &buf, I2Cdev::readTimeout, false);
  Serial.print("OOR 0-7:    ");
  Serial.println(buf, BIN);
  
  I2Cdev::readByte(DEVICE_ADDR, ELE8_11_ELEPROX_OOR_STATUS, &buf, I2Cdev::readTimeout, false);
  Serial.print("OOR 8-PROX: ");
  Serial.println(buf, BIN);

  //CDC
  for (int i = 0; i < 13; i++) {
    I2Cdev::readByte(DEVICE_ADDR, ELE0_CURRENT + i, &buf, I2Cdev::readTimeout, false);
    Serial.print("CDC");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(buf & 0xFFF);
  }
  
  //CDT
  for (int i = 0; i < 6; i++) {
    I2Cdev::readByte(DEVICE_ADDR, ELE0_ELE1_CHARGE_TIME + i, &buf, I2Cdev::readTimeout, false);
    Serial.print("CDT");
    Serial.print(i*2+0);
    Serial.print(": ");
    Serial.println(buf & 0b111);
    Serial.print("CDT");
    Serial.print(i*2+1);
    Serial.print(": ");
    Serial.println((buf >> 4) & 0b111);
  }
  I2Cdev::readByte(DEVICE_ADDR, ELE0_ELE1_CHARGE_TIME + 6, &buf, I2Cdev::readTimeout, false);
  Serial.print("CDT");
  Serial.print(12);
  Serial.print(": ");
  Serial.println(buf & 0b111);
}
*/

#endif