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

#include "ClydeV2_ColorLight.h"
#include "ClydeV2_ColorTypes.h"
#include "ClydeV2_DebugFlags.h"

/**
 * Constructor
 */
ClydeV2_ColorLight::ClydeV2_ColorLight()
  : redLight(ClydeV2_Light()),
  greenLight(ClydeV2_Light()),
  blueLight(ClydeV2_Light()),
  colorCycle(ClydeV2_ColorCycle())
{
  //Init
  color = RGB(0, 0, 0);
  savedColor = RGB(0, 0, 0);
  targetColor = RGB(0, 0, 0);  
}

/**
 * Initaliaze the Color Light
 */
bool ClydeV2_ColorLight::init(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin, RGB _initColor) {

  //Initial color
  color = _initColor;
  savedColor = _initColor;

  //Initialize the Lights with the pins numbers.
  redLight.init(r_pin);
  greenLight.init(g_pin);
  blueLight.init(b_pin);

  //set default lights off
  setColor(RGB(0,0,0));

  //Initialize the color cycle
  colorCycle.init(this);

  #ifdef CLYDE_DEBUG_LIGHT
    Serial.println("Clyde: Color Light initialized.");
  #endif
  
  return true;
}

/**
 * Set Color, in RGB, wipes out fade stepping.
 */
void ClydeV2_ColorLight::setColor(const RGB &c) {

  #ifdef CLYDE_DEBUG_COLORLIGHT
    Serial.println("ClydeV2_ColorLight::setColor(const RGB &c)");
    Serial.println(color.r);
    Serial.println(color.g);
    Serial.println(color.b);
  #endif


  //Set color.
  color = c;
  targetColor = c;

  #ifdef CLYDE_DEBUG_COLORLIGHT
    Serial.println("Clyde ColorLight- constrain color to prevent ir blow out");
    Serial.println(ClydeV2_ColorLight::SCALE_CONSTRAINT);
    Serial.println(color.r * ClydeV2_ColorLight::SCALE_CONSTRAINT);
    Serial.println(color.b * ClydeV2_ColorLight::SCALE_CONSTRAINT);
    Serial.println(color.g * ClydeV2_ColorLight::SCALE_CONSTRAINT);
  #endif

  //Pass the new color on to the Lights
  redLight.setBrightness(color.r * ClydeV2_ColorLight::SCALE_CONSTRAINT);
  blueLight.setBrightness(color.b * ClydeV2_ColorLight::SCALE_CONSTRAINT);
  greenLight.setBrightness(color.g * ClydeV2_ColorLight::SCALE_CONSTRAINT);

  show();
}

/**
 * Set Color, in HSV, wipes out fade stepping.
 */
void ClydeV2_ColorLight::setColor(const HSV &hsv) {

  #ifdef CLYDE_DEBUG_COLORLIGHT
    Serial.println("ClydeV2_ColorLight::setColor(const HSV &hsv)");
    Serial.println(hsv.h);
    Serial.println(hsv.s);
    Serial.println(hsv.v);
  #endif
  

  setColor(colorConverter.HSVtoRGB(hsv));
}

/**
 * Show, set the pin to current color.
 */
void ClydeV2_ColorLight::show() {

  //output new color
  redLight.show();
  blueLight.show();
  greenLight.show();
}


/**
 * update
 */
void ClydeV2_ColorLight::update() {

  #ifdef CLYDE_DEBUG_COLORLIGHT
  //  Serial.println("ColorLight::update()");
  //  Serial.println(colorCycle.isOn());
  #endif

  //update the color cycle
  if (colorCycle.isOn()){
    colorCycle.update();
  }

  //fade each rgb value
  redLight.update();
  greenLight.update();
  blueLight.update();
}

/**
 * fadeTo
 * initiate a fade to specified color.  requires call to update in loop
 */
void ClydeV2_ColorLight::fadeTo(const RGB &c, float spd) {

  #ifdef CLYDE_DEBUG_COLORLIGHT
    Serial.print("Color Light fadeTo");
  #endif

  targetColor = c;
  redLight.fadeTo(targetColor.r * ClydeV2_ColorLight::SCALE_CONSTRAINT, spd);
  greenLight.fadeTo(targetColor.g * ClydeV2_ColorLight::SCALE_CONSTRAINT, spd);
  blueLight.fadeTo(targetColor.b * ClydeV2_ColorLight::SCALE_CONSTRAINT, spd);
}

/**
 * Fade request
 */
void ClydeV2_ColorLight::fadeTo(const HSV &c, float spd) {
  fadeTo(colorConverter.HSVtoRGB(c), spd);
}

/**
 * Turn the light on.
 */
void ClydeV2_ColorLight::turnOn(){
    setColor(savedColor);
}

/**
 * Turn the light off.
 */
void ClydeV2_ColorLight::turnOff(){

    //If the color cycle is on, turn it off.
  if (colorCycle.isOn()){
    colorCycle.stop();
  }

  setColor(RGB(0,0,0));
}

/**
* Check if the ambient light is on.
*/
bool ClydeV2_ColorLight::isOn(){
  return targetColor.r > 0 || targetColor.g > 0 || targetColor.b > 0;
}

/** Save the current color. */
void ClydeV2_ColorLight::save() { 
  if (color.r + color.g + color.b > 10) 
    savedColor = RGB(color.r, color.g, color.b); 
}

float ClydeV2_ColorLight::getHue() {
  //Get Current Hue
  HSV hsv = colorConverter.RGBtoHSV(color);
  return hsv.h; //This is the hue
}



/*
 *  Blink
 *  Maximim number of blinks is: 4  = MAX_CYCLE_LENGTH/2 - 2 
 */
void ClydeV2_ColorLight::startBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks) {

  #ifdef CLYDE_DEBUG_COLORLIGHT
    Serial.println("Color light start blink");
  #endif

  //calculate number of steps needed in the cycle
  uint8_t steps = numBlinks*2 + 1;
  
  //if numBlinks was zero (infinite loop), make space for on/off
  if (steps == 1) steps = 2;

  //check number of step limit
  if (steps > ClydeV2_ColorCycle::MAX_CYCLE_LENGTH) return;

  //set blinks color
  RGB colors[steps];
  uint32_t intervals[steps];

  for(int i = 0; i < steps; i++) {
    if (i%2==1) {
      colors[i] = rgb;
      intervals[i] = offDuration;
    }
    else {
      intervals[i] = onDuration;
    }
  }

  colorCycle.setCycle(steps, &colors[0], &intervals[0], numBlinks==0?LOOP:NO_LOOP);
}

void ClydeV2_ColorLight::stopBlink(){

  #ifdef CLYDE_DEBUG_LIGHT
    Serial.println("Color light stop blink");
  #endif

  colorCycle.stop();
  fadeTo(RGB(0,0,0),0.1);
}