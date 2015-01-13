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

#ifndef __CLYDEMODULEDOCK_H
#define __CLYDEMODULEDOCK_H

/**
 * A module dock on the main Clyde controller board.
 * There are two module positions on the board.
 */
struct ClydeV2_ModuleDock {
  static const uint8_t NUM_MODULES = 2;        /**< Total number of module positions. */
  static const uint8_t ID_REPEAT = 8;          /**< Number of success id required to identify a module. */ //TODO adjust this
  ClydeV2_Module* module;                        /**< Identified module. NULL if none. */
  uint8_t dpin;                                /**< Digital pin of this module's position. */
  uint8_t apin;                                /**< Analog pin of this module's position. */
  ClydeV2_Module* idLast;                        /**< Module identified last time we checked. */
  uint8_t idCount;                             /**< Number of times idLast was identified in a row. */
  
  // this finds the analog id value, we have to turn pins on and off in 
  // a special order apparently.
  uint16_t getModuleId() {

    //Set the digital pin to high to set the module to id mode.  
    pinMode(dpin, OUTPUT);
    digitalWrite(dpin, HIGH);
    //Read the module id from the analog pin
    uint16_t idValue = analogRead(apin);
    //Set the digital pin to input to end id mode.
    pinMode(dpin, INPUT);

    return idValue;
  }

};

#endif