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

#include "ClydeV2_ColorCycle.h"
#include "ClydeV2_ColorLight.h"
#include "ClydeV2_DebugFlags.h"

/**
 * Constructor
 */
ClydeV2_ColorCycle::ClydeV2_ColorCycle() 
  : isCycling(false),
  numSteps(0),
  step(0),
  stepStart(0),
  stepEnd(0),
  stepColor(RGB(0, 0, 0)),
  loop(NO_LOOP){
    memset((void*)&colors[0], 0, sizeof(RGB)*ClydeV2_ColorCycle::MAX_CYCLE_LENGTH);
    memset((void*)&intervals[0], 0, sizeof(uint32_t)*ClydeV2_ColorCycle::MAX_CYCLE_LENGTH);
  }

bool ClydeV2_ColorCycle::init(ClydeV2_ColorLight* l){

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ColorCycle::init");
  #endif

  colorLight = l;
}

/**
 * Check if there is an ongoing cycle.
 */
bool ClydeV2_ColorCycle::isOn() {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::isOn");
  #endif

  return isCycling;
}

uint32_t ClydeV2_ColorCycle::getDuration(){

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::getDuration");
  #endif

  uint32_t duration  = 0;

  //Calculate the light cycle duration.
  for(int i = 0; i < numSteps; i++) {

    duration += intervals[i];

  }

  return duration;
}

void ClydeV2_ColorCycle::setCycle(uint8_t steps, const RGB *colors, const uint8_t *intervals, ECycleLoop loop) {
  
  #ifdef CLYDE_DEBUG_CYCLE
  Serial.println("ClydeV2_ColorCycle::setCycle");
  #endif

  for(int i = 0; i < steps; i++)
    this->intervals[i] = *(intervals + i);
  
  setCycle(steps, colors, loop); 

}
void ClydeV2_ColorCycle::setCycle(uint8_t steps, const RGB *colors, const uint16_t *intervals, ECycleLoop loop) {

  #ifdef CLYDE_DEBUG_CYCLE
  Serial.println("ClydeV2_ColorCycle::setCycle uint16_t");
  #endif

 for(int i = 0; i < steps; i++)
    this->intervals[i] = *(intervals + i);
  
  setCycle(steps, colors, loop);  

}
void ClydeV2_ColorCycle::setCycle(uint8_t steps, const RGB *colors, const uint32_t *intervals, ECycleLoop loop) {

  #ifdef CLYDE_DEBUG_CYCLE
  Serial.println("ClydeV2_ColorCycle::setCycle uint32_t");
  #endif

  for(int i = 0; i < steps; i++)
    this->intervals[i] = *(intervals + i);
  
  setCycle(steps, colors, loop);  
}

void ClydeV2_ColorCycle::setCycle(uint8_t steps, const RGB *colors, ECycleLoop loop) {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::setCycle");
  #endif

  this->isCycling = true;//Raise the flag
  this->numSteps = steps;
  
  for(int i = 0; i < steps; i++)
    this->colors[i] = *(colors+i);
  
  this->step = 0;
  this->stepStart = millis();
  this->stepEnd = this->stepStart + this->intervals[this->step];
  this->stepColor = RGB(colorLight->color.r, colorLight->color.g, colorLight->color.b);
  this->loop = loop;

} 

void ClydeV2_ColorCycle::stop() {  //TODO should this be a function pointer set when starting cycle

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::stop()");
  #endif

  isCycling = false;
  //off();  

 /* switch(type) {
    case SUNSET:
      //if current cycle is the afraid of the dark sunset,
      //then speed up and complete cycle
      //TODO setCycle could also set a pointer to a stop function...
      speedUp(100);
      break;
    default:
      off();  
      break;
  } */
}


/**
 * Update the cycle.
 */
void ClydeV2_ColorCycle::update() {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::update");
  #endif

  uint32_t now = millis();
  
  // If we reach the end of the current step
  if (now > this->stepEnd) {
    // Process to next step
    updateNextStep(now);
    // If we reached the end, then we're done
    if (!this->isOn()){
      return;
    }
  }

  // Find where we are in the step 0-1
  float t;
  t = this->stepEnd - this->stepStart;
  t = t == 0 ? 1 : (now - this->stepStart) / t;

  // Calculate the color for t
  RGB newColor;
  int16_t diff;
  diff = this->colors[this->step].r - this->stepColor.r;
  newColor.r = this->stepColor.r + (t*diff);
  diff = this->colors[this->step].g -this->stepColor.g;
  newColor.g = this->stepColor.g + (t*diff);
  diff = this->colors[this->step].b - this->stepColor.b;
  newColor.b = this->stepColor.b + (t*diff);
  
  colorLight->setColor(newColor);
}

/**
 * Update the cycle to the next step.
 */
void ClydeV2_ColorCycle::updateNextStep(uint32_t now) {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::updateNextStep");
  #endif

  this->step++;

  // Stop cycle if we reached the end
  if (this->step >= this->numSteps) {
    if (this->loop == LOOP) {
      this->step = 0;
    }
    else {
     // This->type = OFF;
      colorLight->setColor(this->colors[this->numSteps-1]);
      this->stop();
      return;
    }
  }

  this->stepStart = now;
  this->stepEnd += this->intervals[this->step];
  this->stepColor = RGB(colorLight->color.r, colorLight->color.g, colorLight->color.b);
}

/**
 * Set the cycle step.
 */
void ClydeV2_ColorCycle::setCycleStep(uint8_t step) {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::setCycleStep");
  #endif

  if (step >= this->numSteps) return;
  
  this->step = step;
  this->stepStart = millis();
  this->stepEnd += this->intervals[this->step];
}

uint8_t ClydeV2_ColorCycle::getCycleStep( ){

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::getCycleStep");
  #endif

  return this->step;
}

/**
 * Next step.
 */
void ClydeV2_ColorCycle::nextStep(uint32_t now) {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::nextStep");
  #endif

  this->stepEnd = now;
  updateNextStep(now);
}

/**
 * Speed up cycle.
 */
void ClydeV2_ColorCycle::speedUp(uint32_t factor) {

  #ifdef CLYDE_DEBUG_CYCLE
    Serial.println("ClydeV2_ColorCycle::speedUp");
  #endif

  // Reduce interval time to speed up end of cycle
  for(int i = 0; i < this->numSteps; i++) {
    this->intervals[i] /= factor;
    factor *= 2;
  }
  
  // Jump cycle to next color
  nextStep(millis());
}