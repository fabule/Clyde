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

#ifndef __CLYDE_COLORLIGHT_H
#define __CLYDE_COLORLIGHT_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ClydeV2_ColorTypes.h"
#include "ClydeV2_Light.h"
#include "ClydeV2_ColorCycle.h"

/**
 * The Color RGB Light
 * A composite of three Light objects, one for red, green and blue.
 * and a memory of its current and target color.
 * Ability to fade and blink.
 */
class ClydeV2_ColorCycle;
class ClydeV2_ColorLight {

public:
  static const float SCALE_CONSTRAINT = 0.88f; //225.0f / 255.0f;  // < Intensity constraint because of conflict with eye's IR sensor.
  
  ClydeV2_Light redLight;
  ClydeV2_Light greenLight;
  ClydeV2_Light blueLight;

  RGBf color;                           // < Current color.
  RGB targetColor;                      // < Target color, used for fading.
  RGB savedColor;                       // < Saved ambient color to go back to.
  float hue;
  ColorConverter colorConverter;
  ClydeV2_ColorCycle colorCycle;

public:  
  /** Constructor. */
  ClydeV2_ColorLight();

  /**  Initialize the module with specified analog and digital pins.*/  
  bool init(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin, RGB _initColor);
  /** Update the color light.  Call in loop to Handle fading light.  */  
  void update();
  /** Send the color values to the color light pins. */  
  void show();

  /** Turn the light on. */
  void turnOn();
  /** Turn the light off. */
  void turnOff();
  /** Check if the light is on.*/
  bool isOn();

  /** Set the color of the light. */
  void setColor(const RGB &c);
  void setColor(const HSV &c);
  /** Fade the color light to a given color. */
  void fadeTo(const RGB &c, float spd);
  void fadeTo(const HSV &c, float spd);
  /** Save the current color. */
  void save();
    /** Get the current hue of the color light. */
  float getHue();

  /** Blink the light */
  void startBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks);
  void stopBlink();

};

#endif