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
#include "ClydeV2_DebugFlags.h"
#include "ClydeV2_SquishyEye.h"
#include "ClydeV2_Config.h"
#include "ClydeV2.h"

bool ClydeV2_SquishyEye::init(uint8_t _pin){

  pin = _pin;
  
  #ifdef CLYDE_DEBUG_EYE
    restartCount = 0;
  #endif
}

void ClydeV2_SquishyEye::update(ClydeV2* c) {

  #ifdef CLYDE_DEBUG_EYE
    //Serial.println("SquishyEye::update");
    //Serial.print("once Calibrated: ");
    //Serial.println(onceCalibrated);
  #endif

  //read IR value
  uint16_t irValue = analogRead(pin);

  // Calibrated the eye's IR sensor
  calibrate(c, irValue);

  // Check if the eye is pressed
  if(onceCalibrated)
    isPressed(c, irValue);
}


void ClydeV2_SquishyEye::calibrate(ClydeV2* c, uint16_t irValue) {  
  //check if calibration was locked
  if (millis() < calibLock) return;
  
  //if the eye is pressed, don't try to calibrate
  if (pressedCount > 0) return;
  
  //get difference since last time
  int32_t irDiff = irValue > irLast ? irValue - irLast : irLast - irValue;

  //check if the IR value is stable, if so continue collecting samples
  if (irDiff < ClydeV2_SquishyEye::CALIB_MAX_RANGE) {
    calibCount++;
    irMin = irValue < irMin ? irValue : irMin;
    irMax = irValue > irMax ? irValue : irMax;
  }
  //if the value is too far from the previous, restart calibration
  else {
    calibCount = 0;
    irMin = irMax = irValue;
    
    #ifdef CLYDE_DEBUG_EYE
      restartCount++;
    #endif
  }

  //check if we've read enough samples to calibrate
  if (calibCount >= ClydeV2_SquishyEye::CALIB_NUM_REPEATS) {
    
    //check if the range of values is beyond the threshold, if so then restart
    if (irMax-irMin >= ClydeV2_SquishyEye::CALIB_MAX_RANGE) {
      calibCount = 0;
      irMin = irMax = irValue;
      return;
    }
    
    //average ir reading
    uint16_t irAvg = (irMin + irMax) / 2;
    
    //only calibrate if the threshold is above a certain limit
    //if not it's too unpredictable (e.g. the sun is shining on it)
    if (irAvg < (uint16_t)((ClydeV2_SquishyEye::CALIB_FORMULA_B - ClydeV2_SquishyEye::CALIB_MIN_THRESHOLD_DIFF) / ClydeV2_SquishyEye::CALIB_FORMULA_A)) {
      //if the eye was not calibrated, turn on ambient light to show feedback
      if (!calibrated)
        c->ambientLight.fadeTo(c->ambientLight.savedColor, 0.1f);
      
      if (!onceCalibrated)
        c->taskLight.setBrightness(255);

      calibrated = true;
      onceCalibrated = true;

      //calculate the threshold
      //simple conversion from detected base ir to threshold
      //the less ir detected (higher value) the less difference required to trigger
      uint16_t newThreshold = irAvg * ClydeV2_SquishyEye::CALIB_FORMULA_A + ClydeV2_SquishyEye::CALIB_FORMULA_B;

      #ifdef CLYDE_DEBUG_EYE
        if (irThreshold != newThreshold) {
          Serial.print("Clyde: eye calibrated. avg = ");
          Serial.print(irAvg);
          Serial.print(", threshold = ");
          Serial.print(newThreshold);
          Serial.print(", range = ");
          Serial.print(irMax - irMin);
          Serial.print(", noisy restarts = ");
          Serial.println(restartCount);
        }
        restartCount = 0;
      #endif      
     
      irThreshold = newThreshold;
    }
    //if there's NOT enough IR emitted by the circuit to recalibrate, then set to recalibrate
    else if (calibrated) {

      calibrated = false;
      
      #ifdef CLYDE_DEBUG_EYE
      Serial.print("Clyde: eye uncalibrated. not enough IR detected, check circuit. ir = ");
      Serial.print(irAvg);
      Serial.print(", minimum = ");
      Serial.println((uint16_t)((ClydeV2_SquishyEye::CALIB_FORMULA_B - ClydeV2_SquishyEye::CALIB_MIN_THRESHOLD_DIFF) / ClydeV2_SquishyEye::CALIB_FORMULA_A));
      #endif
    }
    
    //reset values
    calibCount = 0;
    irMin = 1025;
    irMax = 0;
  }

  irLast = irValue;
}

bool ClydeV2_SquishyEye::isPressed(ClydeV2* c, uint16_t irValue) {

  //require that IR is calibrated
  if (!calibrated || millis() < pressLock) return false;

  #ifdef CLYDE_DEBUG_EYE
  //Serial.print("Clyde: IR = ");
  //Serial.println(irValue);
  #endif
  
  //if the eye press is detected enough time, trigger press event
  if (pressedCount == ClydeV2_SquishyEye::PRESS_COUNT_THRESHOLD) {
    //and we detect that's it's still pressed,
    //then keep track of the last time is was detected
    if (irValue >= irThreshold) {
      pressedLast = millis();
      //if the eye has been pressed for a some time, auto release
      if (millis() > pressedStart+3000) {
        pressedCount = 0;
        calibLock = pressLock = millis() + 1500;

        #ifdef CLYDE_DEBUG_EYE
        Serial.println("Clyde: eye long press detected. auto release.");
        #endif
      }
    }
    //if it's not pressed, and it's been a bit of time, then release
    else if (millis() > pressedLast+200) {
      pressedCount = 0;
    }
  }
  //if the eye is pressed, increase detection count
  else if (irValue >= irThreshold) {

    pressedCount++;
    pressedLast = millis();

    if (pressedCount == ClydeV2_SquishyEye::PRESS_COUNT_THRESHOLD) {

      pressedStart = pressedLast;
      onPress(c);
      return true;

    }
  }
  //if it's been some time since the last detection of a released state, then release
  else if (millis() > pressedLast+500) {
    pressedCount = 0;
  }

  return false;
}

void ClydeV2_SquishyEye::onPress(ClydeV2* c){

    #ifdef CLYDE_DEBUG_EYE
      Serial.println("SquishyEye::onPress");
    #endif

    c->onPress();

    //Call the press event handler.
    if(pressedHandler) pressedHandler();
}

/*
void ClydeV2_SquishyEye::onRelease(ClydeV2* c){

    #ifdef CLYDE_DEBUG_EYE
      Serial.print("ClydeV2_SquishyEye::OnRelease()");
    #endif

    //Call the release event handler.
    if(releasedHandler) releasedHandler();
}*/




    
/*
    if (pressedCount == SquishyEye::PRESS_COUNT_THRESHOLD) {

    //The eye has been pressed for long enough to consider it a true button press.

    // Is the button still pressed? Does the ir value meet the threshold?
    if (irValue >= irThreshold) {

      pressedLast = millis();

      // Press has been detected, but not released.  
      // If enough time has passed, auto release 
      if (millis() >= pressedStart + 3000 ) {

        // Time to autorelease.
        // Lock the calibraion and press detection locks, and hope that the user takes their hand off
        // the button before we try to detect the button press again.

        pressedCount = 0;
        calibLock = pressDetectLock = millis() + 1500;
       // onRelease(c);

        //blink(RGB(255,0,0), 200, 200, 3);
       //setAmbient(RGB(0, 0, 0));
        //setWhite(255);
        
        //setPlayMode(PLAYMODE_SINGLE);
        //play(SND_ERROR);
      
        #ifdef CLYDE_DEBUG
          Serial.println("Clyde: eye long press detected. auto release.");
        #endif
      }
    }
    //if it's not pressed, and it's been a bit of time, then release
    else if (millis() > pressedLast + 200) {
      pressedCount = 0;
    }
  }
  //if the eye is pressed, increase detection count
  else if (irValue >= irThreshold) {


    // The button is pressed, but we must detected this stated numerous time to prevent false triggers.

// Increment the press counter and store the time of the press detection
    pressedCount++;
    pressedLast = millis();
    // If the eye is pressed, and has been pressed long enough to consider it a true press,
    // then we have a button press.  Trigger the action!
    if (pressedCount == SquishyEye::PRESS_COUNT_THRESHOLD) {

      //The button is pressed!

      //Store the time of this press
      pressedStart = pressedLast; 
      //Trigger response
      onPress(c);

      return true;
    }
  }
  //if it's been some time since the last detection of a released state, then release
  else if (millis() > pressedLast + 500) {

    // The button is not pressed, and it's been a bit of time, 
    // then reset pressedCount to get ready for the next press
    pressedCount = 0;
  }












  //////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////


  void SquishyEye::calibrate() {

  #ifdef CLYDE_DEBUG_EYE
    //Serial.println("SquishyEye::calibrate");
  #endif
 
  // In order for the SquishyButton to function we must calibrate the button
  // to work in Clyde's current lighting conditions.  To do this we will collect
  // a series of ir sensor values and determine the max and min ir sensor values.
  // From these we have a profile of the SquishyButton's resting state.  From this 
  // resting state we determine a threshold value, irThreshold 
  // A button press is detected by comparing current ir sensor values to irThreshold 

  //Do not proceed with calibration, if calibration is locked.
  if (millis() < calibLock) return;

  //If the eye is pressed, don't try to calibrate
  if (pressedCount > 0) return;

  //Read the IR sensor's current
  irValue = analogRead(pin);

  //If this is the first time through, set the irLast to the current irValue
  if(irLast == 0){
    irLast = irValue;
  }

  #ifdef CLYDE_DEBUG_EYE
    //Serial.print("Clyde: IR = ");
    //Serial.println(irValue);
  #endif

  //Check if the IR sensor values are stable.
  
  //Get difference between the current and the last ir sensor value. 
  int32_t irDiff = irValue > irLast ? irValue - irLast : irLast - irValue;

  #ifdef CLYDE_DEBUG_EYE
    //Serial.print(" IR last ");
    // Serial.print(irLast);
    // Serial.print("Clyde: IR diff = ");
    // Serial.println(irDiff);
  #endif

  //If the difference is within the accepted range then collect current ir sample.
  if (irDiff < SquishyEye::CALIB_MAX_RANGE) {
    calibCount++;
    irMin = irValue < irMin ? irValue : irMin;
    irMax = irValue > irMax ? irValue : irMax;
  }
  //if the value is too far from the previous, restart calibration
  else {
    calibCount = 0;
    irMin = irMax = irValue;
   // resetCalibration(); //can i replace this else clause with this call?
    return;
  }

  //Once we've collected enough sample ir values we can set the button's resting state.
  if (calibCount >= SquishyEye::CALIB_NUM_REPEATS) {
    
    //Check if the range of ir values within the acceptable range size.  If not, restart calibration.
    if (irMax-irMin >= SquishyEye::CALIB_MAX_RANGE) {
      resetCalibration();
      return;
    }
    
    //Calculate the threshold ir sensor value for the SquishyButton.
    //When the ir sensor value reaches the button is pressed, and we can trigger onPress behaviors.

    //Find the middle of the resting state range.
    uint16_t irAvg = (irMin + irMax) / 2;
    
    //only calibrate if the threshold will be above a certain limit
    //if not it's too unpredictable (e.g. the sun is shining on it)
    if (irAvg < (uint16_t)((SquishyEye::CALIB_FORMULA_B - SquishyEye::CALIB_MIN_THRESHOLD_DIFF) / SquishyEye::CALIB_FORMULA_A)) {
      */

    //if the eye was not calibrated, turn on ambient light to show feedback
    /*  if (!calibrated)
        fadeAmbient(savedColor, 0.1f);
      
      if (!onceCalibrated)
        setWhite(255);
*/
/*
      isCalibrated = true;
      onceCalibrated = true;

      //calculate the threshold
      //simple conversion from detected base ir to threshold
      //the less ir detected (higher value) the less difference required to trigger
      uint16_t newThreshold = irAvg * SquishyEye::CALIB_FORMULA_A + SquishyEye::CALIB_FORMULA_B;

      #ifdef CLYDE_DEBUG_EYE
        if (irThreshold != newThreshold) {
          Serial.print("Clyde: eye calibrated. avg = ");
          Serial.print(irAvg);
          Serial.print(", threshold = ");
          Serial.print(newThreshold);
          Serial.print(", range = ");
          Serial.print(irMax - irMin);
          Serial.print(", noisy restarts = ");
          Serial.println(restartCount);
        }
        restartCount = 0;
      #endif      
     
      irThreshold = newThreshold;
    }
    //if there's NOT enough IR emitted by the circuit to recalibrate, then set to recalibrate
    else if (isCalibrated) {

      isCalibrated = false;

      //Error State, Breathing....
      //blink(RGB(255, 0, 0), 200, 200, 3);
      //setAmbient(RGB(0, 0, 0));
      //setWhite(255);
      
      //setPlayMode(PLAYMODE_SINGLE);
      //play(SND_ERROR);
      
      #ifdef CLYDE_DEBUG
        Serial.print("Clyde: eye uncalibrated. not enough IR detected, check circuit. ir = ");
        Serial.print(irAvg);
        Serial.print(", minimum = ");
        Serial.println((uint16_t)((SquishyEye::CALIB_FORMULA_B - SquishyEye::CALIB_MIN_THRESHOLD_DIFF) / SquishyEye::CALIB_FORMULA_A));
      #endif
    }
    
    resetCalibration();
  }

  //Store the current ir value.
  irLast = irValue;
}

bool SquishyEye::isPressed(ClydeV2* c) {

  bool isPressed = false;

    #ifdef CLYDE_DEBUG_EYE
      //Serial.println("SquishyEye::isPressed");
      //Serial.print( "pressedStart ");
      //Serial.println(pressedStart);
    #endif

    // Button presses are detected by comparing the current ir sensor value to
    // the threshold that was set during calibration.  In order to prevent false positives
    // we will check that the threshold had been reached in multiple sequential sensor readings.
    // Once the threshold has been reached the required number of times, the button press
    // event handling is triggered.


    // Is the button calibrated? Is the press check locked?
    // Do not check for button press until the button has been calibrated and unless 
    // enough time has passed since Clyde booted.
    if (!isCalibrated || millis() < pressDetectLock) return false;

    // What is the current IR sensor value?
    // Read the current value of the IR sensor
    irValue = analogRead(pin);

    #ifdef CLYDE_DEBUG_EYE
      Serial.print("Clyde: IR = ");
      Serial.print(irValue);
      Serial.print("Clyde: IR threshold = ");
      Serial.println(irThreshold);
    #endif

//-----------------------------



  //if the eye press is detected enough time, trigger press event
  if (pressedCount == SquishyEye::PRESS_COUNT_THRESHOLD) {
    //and we detect that's it's still pressed,
    //then keep track of the last time is was detected
    if (irValue >= irThreshold) {
      pressedLast = millis();


      //if the eye has been pressed for a some time, auto release
            //SquishyEye::PRESS_RELEASE_TIMER
      if (millis() >= pressedStart + 3000 ) {

        pressedCount = 0;
        calibLock = pressDetectLock = millis() + 1500;

        //blink(RGB(255,0,0), 200, 200, 3);
       //setAmbient(RGB(0, 0, 0));
        //setWhite(255);
        
        //setPlayMode(PLAYMODE_SINGLE);
        //play(SND_ERROR);
      
        #ifdef CLYDE_DEBUG
          Serial.println("Clyde: eye long press detected. auto release.");
        #endif
      }
    }
    //if it's not pressed, and it's been a bit of time, then release
    else if (millis() > pressedLast + 200) {
      pressedCount = 0;
    }
  }
  //if the eye is pressed, increase detection count
  else if (irValue >= irThreshold) {

    pressedCount++;
    pressedLast = millis();

    if (pressedCount == SquishyEye::PRESS_COUNT_THRESHOLD) {

      //The button is pressed!
      pressedStart = pressedLast;
      isPressed = true;
      onPress(c);
    }
  }
  //if it's been some time since the last detection of a released state, then release
  else if (millis() > pressedLast + 500) {
    pressedCount = 0;
  }
*/

//-----------------------------

/*

    // Is the button pressed? Does the ir value meet the threshold?
    if (irValue >= irThreshold) {

        // The button is pressed, but we must detected this stated numerous time to prevent false triggers.

        #ifdef CLYDE_DEBUG_EYE
          Serial.println("irValue above threshold");
        #endif

        // Increment the press counter and store the time of the press detection
        pressedCount++;
        pressedLast = millis();

        // If the button is active for a given number of times, we consider it a button press.
        if (pressedCount == SquishyEye::PRESS_COUNT_THRESHOLD) {

          #ifdef CLYDE_DEBUG_EYE
            Serial.println("pressedCount equals PRESS_COUNT_THRESHOLD");
            Serial.print(millis());
            Serial.print( " > ");
            Serial.println(pressedStart + SquishyEye::PRESS_RELEASE_TIMER);
            Serial.print( "pressedStart ");
            Serial.println(pressedStart);

          #endif

          // Check if the button press has already been detected,
          // but not released.  If enough time has passed, auto release 
          if (millis() > pressedStart + SquishyEye::PRESS_RELEASE_TIMER) {

            //We need to check and make sure that the user has not continued to press after we have triggered the press event.
            //If it is still pressed, lock the press detection and calibraion locks, and hope that the user takes their hand off
            //the button before we try to detect the button press again.
            autoRelease(c);

          }else{

              //The button is pressed!
              onPress(c);
              return true;
          }
        
        }

      //if it's been some time since the last detection of a released state, then release
      }else if (millis() > pressedLast+200){
        //The button is not pressed, and it's been a bit of time, 
        // then reset pressedCount to get ready for the next press
        pressedCount = 0;
      }
*/
/*
  return isPressed;
}
*/