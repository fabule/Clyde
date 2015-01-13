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

#include "ClydeV2_TouchyFeely.h"
#include "I2Cdev.h"

#ifdef ENABLE_TOUCHY_FEELY

ClydeV2_TouchyFeely TouchyFeely;

bool ClydeV2_TouchyFeely::init(uint8_t _apin, uint8_t _dpin) {

  apin = _apin;
  dpin = _dpin;

  // Test the mpr121 connection
  if (!mpr121.testConnection()) {
    #ifdef CLYDE_DEBUG_TF
      Serial.println("Failed to initialize Touchy-Feely personality. Failed to connect to MPR121.");
    #endif
    return false;
  }
  
  // Initialize the mpr121
  mpr121.initialize(false);
 
  // Initialize the digital pin
  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);

  // Initialize individual leg touch tracking arrays.
  for(int i=0; i < ClydeV2_TouchyFeely::NUM_LEGS; i++) {
    legTouched[i] = false;
    legStartTouchTime[i] = 0;
  }
  
  #ifdef CLYDE_DEBUG_TF
    Serial.println("Touchy-Feely personality initialized.");
  #endif
  
  return true;
}

void ClydeV2_TouchyFeely::update(ClydeV2* c) {

  #ifdef CLYDE_DEBUG_TF
    //Serial.println("Clyde: Touchy-Feely update.");
  #endif

  resetMPR121(c);

  if(c->showPersonality){

    // Touchy Feely is only active if the ambient light is on.
    // Check if ambient light is on.  If it is not, return
    if (!c->ambientLight.isOn() /* || c->taskLight.isOn()*/) return;
  }

  // Confirm a previous touch event, 
  // Trigger touch event a few milliseconds after the touch was first detected to prevent false positive
  if ((touchStatus & 0x0FFF) && (millis()-touchStart > ClydeV2_TouchyFeely::TOUCH_CONFIRM_DURATION)) {

    #ifdef CLYDE_DEBUG_TF
     // Serial.println("Clyde: Touchy-Feely triggered touch event.");
    #endif

    startTouchAction(c);
    
    //reset status to only call this once
    touchStatus = 0x1000;
  }

  // Check for mpr121 interrupt
  if (digitalRead(dpin))
    return;
  
  // Read the touch state from the MPR121
  touchStatus = mpr121.getTouchStatus();

  Serial.println(touchStatus);

  // Check if a leg is touched.
  // Keep track of when touch started. Touch will be confirmed before touch confirmed.
  if (touchStatus & 0x0FFF) {

    // Leg Touched
    #ifdef CLYDE_DEBUG_TF
    //  Serial.println("Clyde: Touchy-Feely detected a touch.");
    #endif
    
    touchStart = millis();

  } else {

    // Leg Released
    #ifdef CLYDE_DEBUG_TF
    //  Serial.print("Clyde: Touchy-Feely detected a release. Touch lasted: ");
    //  Serial.println(millis() - touchStart);
    #endif

    stopTouchAction(c);
  }
  
  // Call released handler if it is set and no legs are touched
  if( releasedHandler && !(touchStatus & 0x0FFF))
    releasedHandler();

  // Check for individual leg touch and release events
  //updateLegs(c);
}

void ClydeV2_TouchyFeely::updateLegs(ClydeV2* c) {

  #ifdef CLYDE_DEBUG_TF
    //Serial.println("Clyde Touchy-Feely: updateLegStatus");
  #endif

  // For each leg we check for touches.
  // We register the detection of a new touch during one update loop,
  // the next update loop, we register the touch again as a touch confirmation, 
  // and call the touch handler
  // Finally we register when a touch is released

  // Restart the count of active leg touches.
  numLegsTouched = 0;

  // One leg at a time, check the leg touch status, and trigger callbacks, if set
  for(int i=0; i < ClydeV2_TouchyFeely::NUM_LEGS; i++) {

    bool legTouchStatus = mpr121.getTouchStatus(i);

    if(legTouchStatus){
      numLegsTouched++;
    }

    processLegTouchStatus(i, legTouchStatus);
  }

  // Check the number of legs that are currently touched.
  #ifdef CLYDE_DEBUG_TF
    Serial.print("Touchy-Feely number of legs touched: ");
    Serial.println(numLegsTouched);
  #endif

  // Daisy Check
  if(c->showPersonality){
    // If the correct number of legs are touched, then trigger daisy.
    if(numLegsTouched == ClydeV2_TouchyFeely::DAISY_LEGS){
      daisyAction(c);
    }
  }

}

void ClydeV2_TouchyFeely::processLegTouchStatus(int legNum, bool legTouchStatus) {

    #ifdef CLYDE_DEBUG_TF
      //Serial.print("TouchyFeely::processLegTouchStatus");
    #endif

    // Two step trigger for touch confirmation.
    // First a leg touch is detected, then it is confirmed and classified as a touch.

    // Check for initial touch detection
    if(legTouchStatus == true && legTouched[legNum] == false) {

      if(legStartTouchTime[legNum] == 0) {
        legStartTouchTime[legNum] = millis();
       
        #ifdef CLYDE_DEBUG_TF
        //  Serial.print("Clyde: Touchy-Feely detected - leg: ");
        //  Serial.println(legNum);
        #endif

        if(legDetectedHandler) legDetectedHandler(legNum);
      }

      if(millis()-legStartTouchTime[legNum] >= ClydeV2_TouchyFeely::LEG_START_THRESH) {
        legTouched[legNum] = true;
      }
    }

    // Check for touch confirmation
    // If the leg was touched the last time we checked, and it is now touched
    // then the touch is confirmed.
    else if(legTouchStatus == true && legTouched[legNum] == true) {

        #ifdef CLYDE_DEBUG_TF
      //    Serial.print("Clyde: Touchy-Feely touch - leg: ");
      //    Serial.println(legNum);
        #endif

      if(legTouchedHandler) legTouchedHandler(legNum);
    }

    // Check for touch release
    else if(legTouchStatus == false && legTouched[legNum] == true) {

      if(millis()-legStartTouchTime[legNum] >= ClydeV2_TouchyFeely::LEG_STOP_THRESH) {

        legTouched[legNum] = false;
        legStartTouchTime[legNum] = 0;

        #ifdef CLYDE_DEBUG_TF
       //   Serial.print("Clyde: Touchy-Feely released - leg: ");
        //  Serial.println(legNum);
        #endif

        if(legReleasedHandler) legReleasedHandler(legNum);
      }

    }
}

void ClydeV2_TouchyFeely::tickleCheck(ClydeV2* c) {

  // If there is a given amount of touch detected within an interval, 
  // consider Clyde tickled.

  // Increase the tickle count
  tickleCount++;
  if (tickleCount == 1) {
    firstTickle = millis(); //keep track of the first tickle
  }
  // If the next touch is fast enough, within the tickle interval,
  // then consider this a tickle
  else if (millis() < firstTickle + ClydeV2_TouchyFeely::TICKLE_INTERVAL) {

    //Tickle detected

    // Once enough tickle touches have been detected, trigger laugh
    if (tickleCount >= ClydeV2_TouchyFeely::TICKLE_REPEAT) {

      //Clyde is laughing now
      laughAction(c);

      // Reset tickle count
      firstTickle = 0;

    } else {

      // Not enough touches for laughter yet...
      #ifdef CLYDE_DEBUG_TF
        Serial.print("Tickle detected ");
        Serial.print(tickleCount);
        Serial.println(" tickle(s)");
      #endif
    }
    
  }
  // Next touch is not with the tickle interval.
  // Too much time between touches.
  // Reset tickle count
  else {
    tickleCount = 0;
  }
}

void ClydeV2_TouchyFeely::laughAction(ClydeV2* c) {

  if(c->showPersonality){
    #ifdef CLYDE_DEBUG_TF
      Serial.println("TouchyFeely::laughAction");
    #endif

    c->startLaughBehavior();
  }
}

void ClydeV2_TouchyFeely::daisyAction(ClydeV2* c) {

  if(c->showPersonality){
    #ifdef CLYDE_DEBUG_TF
      Serial.println("TouchyFeely::daisyAction");
    #endif

    c->startDaisyBehavior();
  }
}

void ClydeV2_TouchyFeely::startTouchAction(ClydeV2* c) {

  // Start the Color Select Behavior
  if(c->showPersonality){

    #ifdef CLYDE_DEBUG_TF
      Serial.println("TouchyFeely::startTouchAction");
    #endif

    c->startSelectBehavior();
  }

  // Call touched handler if any
  if (touchedHandler) touchedHandler();
}

void ClydeV2_TouchyFeely::stopTouchAction(ClydeV2* c) {
    
    if(c->showPersonality){

      #ifdef CLYDE_DEBUG_TF
        Serial.println("TouchyFeely::stopTouchAction");
      #endif

      /* MUST REPLACE THIS WITH A CHECK TO THE CURRENT BEHAVIOUR TYPE.  CHECK IF IT IS NOT LaughBehavior
       if (!Clyde.ambientLight.colorCycle.is(LAUGH))
            stopColorSelect();
      */
      c->endBehavior();

      // Check for a tickle, if the task light is off
      if (!c->taskLight.isOn()){
        tickleCheck(c);
      }
    }
  }

void ClydeV2_TouchyFeely::resetMPR121(ClydeV2* c) {

    // Reset the MPR121 when light status changes
    if (c->ambientLight.isOn() != lastAmbientOn || c->taskLight.isOn() != lastWhiteOn) {

      //  Reset the MPR121 levels according to task light state.
      if (c->taskLight.isOn())
        mpr121.reset(false, ClydeV2_TouchyFeely::TOUCH_LEVEL*8, ClydeV2_TouchyFeely::RELEASE_LEVEL*2);
      else
        mpr121.reset(false, ClydeV2_TouchyFeely::TOUCH_LEVEL, ClydeV2_TouchyFeely::RELEASE_LEVEL);

      // Store the current state of Clyde's ambient and task lights.
      lastAmbientOn = c->ambientLight.isOn();
      lastWhiteOn = c->taskLight.isOn();
    }
}

/*
void ClydeV2_TouchyFeely::debugAutoConfig() {
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