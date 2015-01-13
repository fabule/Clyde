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

#include "ClydeV2_Light.h"
#include "ClydeV2_DebugFlags.h"

ClydeV2_Light::ClydeV2_Light() {

  brightness = 255;       /**< Current brightness. */
  targetBrightness = 255; /**< Target brightness, used for fading. */
  fadeSpeed = 0.0;   
}


/**
* Initialize the module with specified analog and digital pins.
*/
bool ClydeV2_Light::init(uint8_t _pin) {
 
  // Setup the light pins
  pin = _pin;	
  pinMode(pin, OUTPUT);
  analogWrite(pin, brightness);
  
  #ifdef CLYDE_DEBUG_LIGHT
  Serial.println("Clyde: Light initialized.");
  #endif

  //init white light
  //we use 254 brightness as a hack to remove the flicker on first fade-in,
  //but this adds a flash on startup instead, which is kinda neat
  //TODO look for a better solution
  setBrightness(255);
  
  return true;
}

/**
 * Turn the light on.
 */
void ClydeV2_Light::turnOn(){
  setBrightness(0);
}

/**
 * Turn the light off.
 */
void ClydeV2_Light::turnOff(){
  setBrightness(255);
}

/**
 * Check if the light is on.
 */
bool ClydeV2_Light::isOn(){ 
  return targetBrightness < 255; 
}


/**
 * Update
 * Update the light state.  Call in loop to controls fading and brightness. 
 */
void ClydeV2_Light::update() {

  // Fade only if reached the target brightness yet.
  if (targetBrightness != brightness){

    // Calculate the next brightness value for fade.
    fadeStep();

    // Output new brightness level to pin
    show();
  }
}

/**
 * Show
 * set pin brightness, maintains fade stepping increments
 */
void ClydeV2_Light::show() {

  #ifdef CLYDE_DEBUG_LIGHT
    Serial.print("Light show: ");
    Serial.println(brightness);
  #endif

  analogWrite(pin, brightness);
}

/**
  * Request a fade to specified brightness.
  * spd controls how many steps/interations it will take
  */
void ClydeV2_Light::fadeTo(uint8_t b, float spd) {
  targetBrightness = b;

  fadeSpeed = (targetBrightness - brightness) / 255.0 * spd;

  #ifdef CLYDE_DEBUG_LIGHT
    //Serial.print("Light fadeTo: fadeSpeed");
    //Serial.println(fadeSpeed);
  #endif

  if (fadeSpeed < 0) fadeSpeed *= -1;
}

void ClydeV2_Light::fadeStep(){

  // Get the difference to the target, and the fade direction
  float diff = targetBrightness - brightness;
  int8_t dir = diff < 0 ? -1 : 1;

  // Fade and limit to the defined max fade speed
  brightness += diff*dir < fadeSpeed ? diff : fadeSpeed*dir;

  #ifdef CLYDE_DEBUG_LIGHT
  //  Serial.print("Light fadeStep diff:");
  //  Serial.print(diff);
  //  Serial.print(" dir: ");
  //  Serial.print(dir);
  //  Serial.print(" brightness: ");
  //  Serial.println(brightness);
  #endif
}

/**
 * Set the brightness of the light.
 * wipes out any fade stepping.
 */
void ClydeV2_Light::setBrightness(uint8_t b) {
  brightness = targetBrightness = b;
  show();
}