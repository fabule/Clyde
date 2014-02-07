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
  
  //void set(uint8_t nr, uint8_t ng, uint8_t nb) { r = nr; g = ng; b = nb; }
  //void set(const RGB &c) { r = c.r; g = c.g; b = c.b; }
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
  
  //void set(float nr, float ng, float nb) { r = nr; g = ng; b = nb; }
  //void set(const RGBf &c) { r = c.r; g = c.g; b = c.b; }
};

#endif