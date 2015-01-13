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

#ifndef __COLORTYPES_H
#define __COLORTYPES_H

const float noHue = -1.0;


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/**
 * RGB color using integer values
 */
struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  
  RGB(uint8_t nr = 0, uint8_t ng = 0, uint8_t nb = 0) { r = nr; g = ng; b = nb; }
  
  RGB& operator =(const RGB& a)
  {
    r = a.r;
    g = a.g;
    b = a.b;
    return *this;
  }

};

/**
 * RGB color using float values
 */
struct RGBf {
  float r;
  float g;
  float b;
  
  RGBf(float nr = 0, float ng = 0, float nb = 0) { r = nr; g = ng; b = nb; }
  
  RGBf& operator =(const RGBf& a)
  {
    r = a.r;
    g = a.g;
    b = a.b;
    return *this;
  }
  
  RGBf& operator =(const RGB& a)
  {
    r = a.r;
    g = a.g;
    b = a.b;
    return *this;
  }

};


/**
 * HSV color
 */
struct HSV {
  float h;
  float s;
  float v;
  
  HSV(float nh = 0, float ns = 0, float nv = 0) { h = nh; s = ns; v = nv; }
  
  HSV& operator =(const HSV& a)
  {
    h = a.h;
    s = a.s;
    v = a.v;
    return *this;
  }
};

/**
 * Color Conversion Utlility
 * Convert between RGB and HSV color schemes.
 */
struct ColorConverter {

  RGB HSVtoRGB(HSV hsv){

    float r,g,b;
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;
    int k;
    float aa, bb, cc, f;

    /*Serial.println("HSVtoRGB----");
    Serial.println("HSV----");
    Serial.println(h);
    Serial.println(s);
    Serial.println(v);
    */

    //HSV and RGB values are int he range from 0 to 1.0
    if( s <= 0.0 )
      r = g = b = v; //Have a gray scale if s=0
    else {
      if( h == 1.0)
        h = 0.0;
      h *= 6.0;
      k = floor(h);
      f =  h-k;
      aa = v * (1.0 - s);
      bb = v * (1.0 - (s *f));
      cc = v * (1.0 - (s * (1.0 -f)));

      switch(k)
      {
        case 0: r=v;  g=cc; b=aa; break;
        case 1: r=bb; g=v;  b=aa; break;
        case 2: r=aa; g=v;  b=cc; break;
        case 3: r=aa; g=bb; b=v;  break;
        case 4: r=cc; g=aa; b=v;  break;
        case 5: r=v;  g=aa; b=bb; break;

      }
    }

   /* Serial.println("RGB----");
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    */

    return RGB(r,g,b);


  }

  HSV RGBtoHSV(RGBf rgb) {
    return RGBtoHSV(RGB((uint8_t)rgb.r, (uint8_t)rgb.g, (uint8_t)rgb.b));
  }

  HSV RGBtoHSV(RGB rgb) {

    float h,s,v;
    float r = (float)rgb.r;
    float g = (float)rgb.g;
    float b = (float)rgb.b;

    /*
    Serial.println("RGBtoHSV----");
    Serial.println("RGB----");
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    */


    //RGB to HSV values are in the range from 0 to 1.0
    float minRGB = min(r, min(g,b)), maxRGB = max(r,max(g,b));
    float deltaRGB = maxRGB - minRGB;

    v = maxRGB;
    if(maxRGB != 0.0)
      s = deltaRGB / maxRGB;
    else
      s = 0.0;

    if(s <= 0.0)
      h = noHue;
    else {
      if(r==maxRGB)
        h=(g-b)/deltaRGB;
      else
        if(g==maxRGB)
          h = 2.0 + (b-r) / deltaRGB;
        else
          if(b==maxRGB)
            h = 4.0 + (r-g) / deltaRGB;

      h*= 60.0;
      if(h < 0.0)
        h += 360.0;
      h /= 360.0;


    }

    /*
    Serial.println("HSV----");
    Serial.println(h);
    Serial.println(s);
    Serial.println(v);
    */
    
    return HSV(h, s, v);

  };

};

#endif
