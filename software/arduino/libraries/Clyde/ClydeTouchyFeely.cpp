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
  : CClydeModule(ID_LOW, ID_HIGH), m_mpr121(DEVICE_ADDR, TOUCH_LEVEL, RELEASE_LEVEL) {
  
  m_touchedHandler = NULL;
  m_releasedHandler = NULL;
  m_touchStart = 0;
  m_lastStopStep = 0;
  m_colorSelectEnabled = true;
  m_tickleCount = 0;
  m_firstTickle = 0;
  m_lastAmbientOn = false;
  m_lastWhiteOn = false;
  COLOR_LEG_1  = RGB( 228,  26,  28 ); // red
  COLOR_LEG_2  = RGB(  55, 126, 255 ); // blue
  COLOR_LEG_4  = RGB(  77, 255,  74 ); // green
  COLOR_LEG_8  = RGB( 200,  78, 200 ); // purple
  COLOR_LEG_16 = RGB( 255, 127,   0 ); // orange
  COLOR_LEG_32 = RGB( 240,   2, 127 ); // pink
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
  //reset the MPR121 when light status changes
  if (Clyde.ambient()->isOn() != m_lastAmbientOn || Clyde.white()->isOn() != m_lastWhiteOn) {
    if (Clyde.white()->isOn())
      m_mpr121.reset(false, TOUCH_LEVEL*8, RELEASE_LEVEL*2);
    else
      m_mpr121.reset(false, TOUCH_LEVEL, RELEASE_LEVEL);
    m_lastAmbientOn = Clyde.ambient()->isOn();
    m_lastWhiteOn = Clyde.white()->isOn();
  }
  
  //only active when the ambient light is on
  if (!Clyde.ambient()->isOn()/* || Clyde.white()->isOn()*/) return;

  //trigger touch event after a few millis to protect from false positive
  if ((m_touchStatus & 0x0FFF) && (millis()-m_touchStart > 250)) {
    #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Touchy-Feely triggered touch event.");
    #endif
      
    //start color selection only if the current cycle isn't laugh or select
    if (!Clyde.cycle()->is(SELECT) && !Clyde.cycle()->is(LAUGH) && !Clyde.cycle()->is(SUNSET))
      startColorSelect();
    
    //call touched handler if any
    if (m_touchedHandler) m_touchedHandler();
    
    //reset status to only call this once
    m_touchStatus = 0x1000;
  }
  
  //check for mpr121 interrupt
  if (digitalRead(dpin))
    return;
  
  //read the touch state from the MPR121
  m_touchStatus = m_mpr121.getTouchStatus();

  //keep track of when touch started
  if (m_touchStatus & 0x0FFF) {
    #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Touchy-Feely detected a touch.");
    #endif
    m_lastStatus = m_touchStatus;
    m_touchStart = millis();
  }
  else {
    #ifdef CLYDE_DEBUG
    Serial.print("Clyde: Touchy-Feely detected a release. Touch lasted: ");
    Serial.println(millis() - m_touchStart);
    #endif

    if (!Clyde.cycle()->is(SELECT))
      tickleCheck();

    if (!Clyde.cycle()->is(LAUGH))
      stopColorSelect();    
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
      Serial.println( m_touchStatus );
    }
    #endif
  }
  //if it's been too long, reset tickle count
  else {
    m_tickleCount = 0;
  }
  // if Clyde is not laughing, we change the color based on the touched leg.
  if( !( Clyde.cycle()->is(LAUGH) | Clyde.cycle()->is(SUNSET) ) ){
    switch (m_lastStatus){
    case 1:
      Clyde.fadeAmbient( COLOR_LEG_1, 2 );
      break;
    case 2:
      Clyde.fadeAmbient( COLOR_LEG_2, 2 );
      break;
    case 4:
      Clyde.fadeAmbient( COLOR_LEG_4, 2 );
      break;
    case 8:
      Clyde.fadeAmbient( COLOR_LEG_8, 2 );
      break;
    case 16:
      Clyde.fadeAmbient( COLOR_LEG_16, 2 );
      break;
    case 32:
      Clyde.fadeAmbient( COLOR_LEG_32, 2 );
      break;
    }
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
#ifdef ENABLE_MOUTH
  Clyde.setPlayMode(PLAYMODE_SINGLE_CYCLE);
  Clyde.play(SND_LAUGH);
#endif
}

void CClydeTouchyFeely::startColorSelect() {
  if (!m_colorSelectEnabled) return;

  #ifdef CLYDE_DEBUG
  Serial.println("Clyde: touchy-feely color selection cycle STARTED");
  #endif
  
  Clyde.setCycle(SELECT, SELECT_STEPS, SELECT_COLORS, SELECT_INTERVALS, LOOP);
  Clyde.setCycleStep(m_lastStopStep);
#ifdef ENABLE_MOUTH  
  Clyde.setPlayMode(PLAYMODE_SINGLE_CYCLE);
  Clyde.play(SND_HAPPY);
#endif
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

#ifdef ENABLE_MOUTH
  //stop audio
  Clyde.stop();
#endif
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
