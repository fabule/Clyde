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
#ifndef __CLYDE_LIGHT_H
#define __CLYDE_LIGHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/**
 * The Light
 */

class ClydeV2_Light {

public: 

  uint8_t pin;            /**< Digital pin to control the brightness. */
  float brightness;       /**< Current brightness. */
  float targetBrightness; /**< Target brightness, used for fading. */
  float fadeSpeed;        /**< Speed, used for fading. */

 
  /** Constructor. */
  ClydeV2_Light();
  
  /**  Initialize the module with specified analog and digital pins. */  
  bool init(uint8_t dpin);
  /**  Update the light state.  Call in loop to controls fading and brightness. */  
  void update();
  /**  Show brightness.  Set the pin. */  
  void show();

  /**  Turn the light on. */
  void turnOn();
  /**  Turn the light off. */
  void turnOff();
  /**  Check if the light is on. */
  bool isOn();

  /**
   * Request a fade to specified brightness.
   * spd controls how many steps/interations it will take
   */
  void fadeTo(uint8_t b, float spd);

  /**  Fade the light to the next step. */
  void fadeStep();

  /**  Set the brightness of the light. */
  void setBrightness(uint8_t b);

  /**  Blink the light */
  //void blink();
  
};

#endif // LIGHT_H
