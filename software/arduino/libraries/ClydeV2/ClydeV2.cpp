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
#include "ClydeV2.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

ClydeV2 Clyde; //The Clyde Singleton

#ifdef ENABLE_CHATTERBOX
  ClydeV2_Chattershield ClydeV2_Chatterbox::chattershield(CHATTER_CS_PIN, CHATTER_DCS_PIN, CHATTER_DREQ_PIN);
#endif

ClydeV2::ClydeV2()
: showPersonality(true),
  enableSquishyEyeButton(true) {

  // Disable all the pins initially.
  pinMode(EYE_PIN, OUTPUT);
  digitalWrite(EYE_PIN, LOW);
  pinMode(R_PIN, OUTPUT);
  digitalWrite(R_PIN, LOW);
  pinMode(G_PIN, OUTPUT);
  digitalWrite(G_PIN, LOW);
  pinMode(B_PIN, OUTPUT);
  digitalWrite(B_PIN, LOW);
  pinMode(TASK_PIN, OUTPUT);
  digitalWrite(TASK_PIN, HIGH);
  pinMode(MODULE_1_DPIN, OUTPUT);
  digitalWrite(MODULE_1_DPIN, LOW);
  pinMode(MODULE_1_APIN, OUTPUT);
  digitalWrite(MODULE_1_APIN, LOW);
  pinMode(MODULE_2_DPIN, OUTPUT);
  digitalWrite(MODULE_2_DPIN, LOW);
  pinMode(MODULE_2_APIN, OUTPUT);
  digitalWrite(MODULE_2_APIN, LOW);
  pinMode(CHATTER_CS_PIN, OUTPUT);
  digitalWrite(CHATTER_CS_PIN, LOW);
  pinMode(CHATTER_DCS_PIN, OUTPUT);
  digitalWrite(CHATTER_DCS_PIN, LOW);
  pinMode(CHATTER_DREQ_PIN, OUTPUT);
  digitalWrite(CHATTER_DREQ_PIN, LOW);

  ClydeV2_SquishyEye squishyEye = ClydeV2_SquishyEye();
  ClydeV2_Light taskLight = ClydeV2_Light();
  ClydeV2_ColorLight ambientLight = ClydeV2_ColorLight();
  ClydeV2_EEPROM eeprom = ClydeV2_EEPROM();

  #ifdef ENABLE_CHATTERBOX
     ClydeV2_Chatterbox chatterbox = ClydeV2_Chatterbox();
  #endif

  // Initialize the module docks
  moduleDocks[0].module = NULL;
  moduleDocks[0].dpin = MODULE_1_DPIN;
  moduleDocks[0].apin = MODULE_1_APIN;
  moduleDocks[0].idLast = NULL;
  moduleDocks[0].idCount = 0;
  moduleDocks[1].module = NULL;
  moduleDocks[1].dpin = MODULE_2_DPIN;
  moduleDocks[1].apin = MODULE_2_APIN;
  moduleDocks[1].idLast = NULL;
  moduleDocks[1].idCount = 0;
}

void ClydeV2::begin() {

  #ifdef CLYDE_DEBUG_WAIT
    delay(5000);
  #endif

  #ifdef ENABLE_CHATTERBOX
    // Initialize the chatterbox audio module
    // Do this ASAP to prevent the audio shield auto playing
    chatterbox.init(CHATTER_NUM_TRACKS);
  #endif

  // Initialize the squishy eye button
  squishyEye.init(EYE_PIN);
  // Initialize the task light
  taskLight.init(TASK_PIN);
  
  // Reset the eeprom
  //eeprom.reset();

  //Read the saved color from the eeprom.
  RGB _initColor;
  eeprom.readAmbientColor(&_initColor);
  // Initialize the ambient light
  ambientLight.init(R_PIN, G_PIN, B_PIN, _initColor);

  // Setup module pins
  pinMode(moduleDocks[0].dpin, INPUT);
  digitalWrite(moduleDocks[0].dpin, LOW);
  pinMode(moduleDocks[1].dpin, INPUT);
  digitalWrite(moduleDocks[1].dpin, LOW);

  // Detect Personality Modules
  detectPersonalities();


  // Check if there are initilization errors.  If there are display them
  if(!showInitErrors()){
    // IF there are no initialization errors, set to default behaviour
    // Initialize behaviour state, set to default behavior
    _behavior = ClydeV2_DefaultBehavior::instance();
    _behavior->start(this);

  }

}

bool ClydeV2::showInitErrors(){
  // Any init errors to display? 
  // If the Chattershield was detected, but is not ready, then blink the ambient light.
   #ifdef ENABLE_CHATTERBOX

    // Initialize the chatterbox audio module
    // Do this ASAP to prevent the audio shield auto playing
    if(chatterbox.detected && !chatterbox.ready){
      #ifdef CLYDE_DEBUG
        Serial.println("Chatterbox detected, but is not ready.  There is a problem with the sd card.");
      #endif

      //Blink the ambient light green.
      startBlinkBehavior(RGB(0,255,0), 1000, 1000, 3);
      return true;
    }

  #endif

  return false;

}

void ClydeV2::update(){

  // Update the squishy eye button
  squishyEye.update(this);

  // Update the lights
  ambientLight.update();
  taskLight.update();

  updatePersonalities();

  // Update current behavior
  _behavior->update(this);
}

void ClydeV2::switchLights(){

  #ifdef CLYDE_DEBUG
    Serial.println("switchLights");
  #endif
    
  //Switch the Lights.
  //Clyde light switchs from:
  //both lights off -> just ambient light ->both on -> just task -> both on

  //If only the ambient light is on
  if (!taskLight.isOn() && ambientLight.isOn()) {

    //Fade on task light
    taskLight.fadeTo(0, 0.1f);

  //If both lights are on
  }else if (taskLight.isOn() && ambientLight.isOn()) {

    //Fade the ambient light off
    ambientLight.fadeTo(RGB(0,0,0), 0.5f);

  //If the only the task light is on
  }else if (taskLight.isOn() && !ambientLight.isOn()) {

    //Fade the task light off.
    taskLight.fadeTo(255, 0.3f);

    //Play turning off sound
    #ifdef ENABLE_CHATTERBOX
      chatterbox.playOff();
    #endif

  //If both lights are off
  }else if (!taskLight.isOn() && !ambientLight.isOn()) {

    //Fade on the ambient light
    ambientLight.fadeTo(ambientLight.savedColor, 0.1f);

    //Play the power up sound
    #ifdef ENABLE_CHATTERBOX
      chatterbox.playOn();
    #endif

  }
  
  #ifdef CLYDE_DEBUG
    Serial.print("Switched lights: white is ");
    Serial.print(taskLight.isOn() ? "ON" : "OFF");
    Serial.print(" ambient is ");
    Serial.println(ambientLight.isOn() ? "ON" : "OFF");
  #endif
}

void ClydeV2::onPress(){
  //Switch between Clyde's light states: off > ambient > ambient + task > task
  if(enableSquishyEyeButton)
    _behavior->interrupt(this);
}

void ClydeV2::changeBehavior(ClydeV2_BehaviorState* s){

  #ifdef CLYDE_DEBUG
    Serial.println("changeBehavior");
  #endif 

  //Change the behavior state
  _behavior = s;

  //Start the new behavior state
  _behavior->start(this);
}

/**
  * Blink Behavior
  **/
bool ClydeV2::startBlinkBehavior(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks){

  #ifdef CLYDE_DEBUG
    Serial.println("startBlinkBehavior");
  #endif

  changeBehavior(ClydeV2_BlinkBehavior::instance());
  _behavior->configBlink(rgb, onDuration, offDuration, numBlinks);
  _behavior->start(this);

  return true;
}

/**
  * Color Select Behavior
  **/
bool ClydeV2::startSelectBehavior(){

  //Only start select behavior from default behavior state
  if( _behavior->isDefault ){
    #ifdef CLYDE_DEBUG
      Serial.println("startSelectBehavior");
    #endif

    changeBehavior(ClydeV2_SelectBehavior::instance());
    _behavior->start(this);
    return true;
  }

  return false;
}

/**
  * Laugh Behavior
  **/
bool ClydeV2::startLaughBehavior(){

  //Only start laugh behavior from default behavior state
  if( _behavior->isDefault ){

    #ifdef CLYDE_DEBUG
      Serial.println("startLaughBehavior");
    #endif

    changeBehavior(ClydeV2_LaughBehavior::instance());
    _behavior->start(this);

    return true;
  }
  return false;
}

/**
  * Daisy Behavior
  **/
bool ClydeV2::startDaisyBehavior(){

  #ifdef CLYDE_DEBUG
    Serial.println("startDaisyBehavior");
  #endif

  //Only start daisy behavior from default behavior state
  if( _behavior->isDefault ){
    changeBehavior(ClydeV2_DaisyBehavior::instance());
    _behavior->start(this);

    return true;
  }
  return false;
}

/**
  * Sunset Behavior
  **/
bool ClydeV2::startSunsetBehavior(){

  if( _behavior->isDefault ){

    #ifdef CLYDE_DEBUG
      Serial.println("startSunsetBehavior");
    #endif

    changeBehavior(ClydeV2_SunsetBehavior::instance());
    _behavior->start(this);
    return true;
  }
  return false;
}

/**
  * Sunrise Behavior
  **/
bool ClydeV2::startSunriseBehavior(){

  if( _behavior->isDefault ){
    #ifdef CLYDE_DEBUG
      Serial.println("startSunriseBehavior");
    #endif

    changeBehavior(ClydeV2_SunriseBehavior::instance());
    _behavior->start(this);
    return true;
  }
  return false;
}


/**
  * Hypno Behavior
  **/
bool ClydeV2::startHypnoBehavior(){

  if( _behavior->isDefault ){
    #ifdef CLYDE_DEBUG
      Serial.println("startHypnoBehavior");
    #endif

    changeBehavior(ClydeV2_HypnoBehavior::instance());
    _behavior->start(this);
    return true;
  }
  return false;
}

/**
  * End all types of Behavior
  **/
void ClydeV2::endBehavior(){

  #ifdef CLYDE_DEBUG
    Serial.println("endBehavior");
  #endif

  _behavior->end(this);
}

void ClydeV2::detectPersonalities() {

  #ifdef CLYDE_DEBUG
    Serial.println("detectPersonalities");
  #endif

  //detect personalities
  for(int i = 0; i <= ClydeV2_ModuleDock::ID_REPEAT; i++) {
    //check each module position
    
    for(int j = 0; j < ClydeV2_ModuleDock::NUM_MODULES; j++) {
      ClydeV2_Module* newModule = NULL;

      uint16_t idValue = moduleDocks[j].getModuleId();
            
      //Determine which type of module is plugged into the module position
      #ifdef ENABLE_AFRAID_OF_THE_DARK
        if (AfraidOfTheDark.compareId(idValue))
          newModule = &AfraidOfTheDark;
      #endif
      #ifdef ENABLE_TOUCHY_FEELY
        if (TouchyFeely.compareId(idValue))
          newModule = &TouchyFeely;
      #endif
        
      //if the detected module is different that last, then reset detection count
      //TODO: reverse this if block to check for equality first
      if (newModule != moduleDocks[j].idLast) {
        moduleDocks[j].idCount = 1;
        moduleDocks[j].idLast = newModule;
        newModule = NULL;
      }
      //if the detected module is the same as the last, then increase detection count
      else if (moduleDocks[j].idCount < ClydeV2_ModuleDock::ID_REPEAT) {
        moduleDocks[j].idCount++;
        newModule = NULL;
      }
      //if the detected module has been the same enough times, then init the module
      else {
        if (moduleDocks[j].idCount == ClydeV2_ModuleDock::ID_REPEAT) {
          //if a module is detected, init
          if (newModule != NULL) {
            if (newModule->init(moduleDocks[j].apin, moduleDocks[j].dpin)) {
              moduleDocks[j].module = newModule;
              moduleDocks[j].idCount++;
            }
          }
          //if no module is detected, the reset module position to default
          else {     
            pinMode(moduleDocks[j].dpin, INPUT);
            digitalWrite(moduleDocks[j].dpin, LOW); 
            
            moduleDocks[j].module = newModule;          
            moduleDocks[j].idCount++; 
          }
        }
      }
    }
  }
  
}

void ClydeV2::updatePersonalities() {
  for(int i = 0; i < ClydeV2_ModuleDock::NUM_MODULES; i++) {
    if (moduleDocks[i].module != NULL)
      moduleDocks[i].module->update(this);
  }
}