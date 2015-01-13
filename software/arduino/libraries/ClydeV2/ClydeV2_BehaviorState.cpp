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
#include "ClydeV2_BehaviorState.h"
#include "ClydeV2_ColorTypes.h"
#include "ClydeV2_DebugFlags.h"


/**********************************************
  * Behavior State
  **/
ClydeV2_BehaviorState::ClydeV2_BehaviorState() 
: isDefault(false),
cycleDuration(0), 
startTime(millis()) {

}
void ClydeV2_BehaviorState::start(ClydeV2*) {}
void ClydeV2_BehaviorState::update(ClydeV2*) {}
void ClydeV2_BehaviorState::interrupt(ClydeV2*) {}
void ClydeV2_BehaviorState::end(ClydeV2*) {}
void ClydeV2_BehaviorState::configBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks) {}
void ClydeV2_BehaviorState::changeBehavior(ClydeV2* c, ClydeV2_BehaviorState* s) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("BehaviorState::changeBehavior");
  #endif

  c->changeBehavior(s);
}

bool ClydeV2_BehaviorState::isComplete() {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("BehaviorState::isComplete");
    Serial.print(millis());
    Serial.print( " > ");
    Serial.println( startTime + cycleDuration );
  #endif

  // Note: If the behavior does not have a duration, millis() will be > startTime,
  // and considered complete.

  return millis() > startTime + cycleDuration;
} 


/**********************************************
  * Default Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_DefaultBehavior::_instance = 0;

ClydeV2_BehaviorState* ClydeV2_DefaultBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_DefaultBehavior;
    _instance->isDefault = true;
  }
  return _instance;
}

void ClydeV2_DefaultBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DefaultBehavior::start");
  #endif
}

void ClydeV2_DefaultBehavior::interrupt(ClydeV2* c) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DefaultBehavior::interrupt");
  #endif

  c->switchLights();
}

void ClydeV2_DefaultBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DefaultBehavior::end");
  #endif

  //Change to DefaultBehavior
  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}


/**********************************************
  * Blink Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_BlinkBehavior::_instance = 0;
RGB ClydeV2_BlinkBehavior::rgb = RGB(0,255,255);//Default cyan blinking
uint32_t ClydeV2_BlinkBehavior::onDuration = 1000; 
uint32_t ClydeV2_BlinkBehavior::offDuration = 1000; 
uint8_t ClydeV2_BlinkBehavior::numBlinks = 0; //Default loop the blink cycle

ClydeV2_BehaviorState* ClydeV2_BlinkBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_BlinkBehavior;
  }
  return _instance;
}

void ClydeV2_BlinkBehavior::configBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks){
  ClydeV2_BlinkBehavior::rgb = rgb;
  ClydeV2_BlinkBehavior::onDuration = onDuration;
  ClydeV2_BlinkBehavior::offDuration = offDuration;
  ClydeV2_BlinkBehavior::numBlinks = numBlinks;
}

void ClydeV2_BlinkBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("BlinkBehavior::start");
  #endif

  c->ambientLight.startBlink(ClydeV2_BlinkBehavior::rgb, ClydeV2_BlinkBehavior::onDuration, ClydeV2_BlinkBehavior::offDuration, ClydeV2_BlinkBehavior::numBlinks);
}


void ClydeV2_BlinkBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("BlinkBehavior::update");
  #endif

  //If the BLink cycle is over, end the blink behavior
  if( !c->ambientLight.colorCycle.isOn() ){
    end(c);
  }
}

void ClydeV2_BlinkBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("BlinkBehavior::end");
  #endif

  //Turn of blink and return to default behavior.
  c->ambientLight.stopBlink();
     //Fade on the ambient light
  c->ambientLight.fadeTo(c->ambientLight.savedColor, 0.1f);
  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}


/**********************************************
  * Select Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_SelectBehavior::_instance = 0;

const RGB ClydeV2_SelectBehavior::COLORS[] = {RGB(255,0,0), RGB(255,255,0), RGB(0,255,0), RGB(0,255,255), RGB(0,0,255), RGB(255,0,255), RGB(255,255,255)};
const uint16_t ClydeV2_SelectBehavior::INTERVALS[] = {1000, 1000, 1000, 1000, 1000, 1000, 1000};
const uint8_t ClydeV2_SelectBehavior::STEPS = 7;

ClydeV2_BehaviorState* ClydeV2_SelectBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_SelectBehavior;
  }
  return _instance;
}

void ClydeV2_SelectBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SelectBehavior::start");
  #endif

  c->ambientLight.colorCycle.setCycle(ClydeV2_SelectBehavior::STEPS, ClydeV2_SelectBehavior::COLORS, ClydeV2_SelectBehavior::INTERVALS, LOOP);
  c->ambientLight.colorCycle.setCycleStep(lastStopStep);

  #ifdef ENABLE_CHATTERBOX
     c->chatterbox.playHappy();
  #endif

}

void ClydeV2_SelectBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SelectBehavior::end");
  #endif

  //Save the cycle step in order to restart at the same place
  lastStopStep = c->ambientLight.colorCycle.getCycleStep();
  
  //Turn of color cycle
  c->ambientLight.colorCycle.stop();

  //Save the current color
  c->ambientLight.save();
  
  #ifdef ENABLE_CHATTERBOX
    //Stop audio
    c->chatterbox.stop();
  #endif

  //Return to default behvavior
  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}


/**********************************************
  * Laugh Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_LaughBehavior::_instance = 0;

ClydeV2_BehaviorState* ClydeV2_LaughBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_LaughBehavior;
  }
  return _instance;
}

void ClydeV2_LaughBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("LaughBehavior::start");
  #endif 

  //we need at least three cycle steps to store tickle
  if (ClydeV2_ColorCycle::MAX_CYCLE_LENGTH < 2) return;

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("Clyde: laughs");
  #endif
 
  //generate random tickle cycle
  //Generate Tickle ColorCycle values.
  //Generate a random number of cycle steps, 
  //generate a random color and interval for each

  steps = random(ClydeV2_ColorCycle::MAX_CYCLE_LENGTH/4,ClydeV2_ColorCycle::MAX_CYCLE_LENGTH/2) * 2;

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("number of laugh steps");
    Serial.println(ClydeV2_LaughBehavior::steps);
  #endif

  uint8_t i;
  for(i = 0; i < ClydeV2_LaughBehavior::steps; i+=2) { 
    ClydeV2_LaughBehavior::colors[i] = RGB(random(60, 100), 0, random(200, 255)); //Generate a random shade of magento
    ClydeV2_LaughBehavior::intervals[i] = random(100, 200); //Generate an interval value between 100 and 200 ms
  
    ClydeV2_LaughBehavior::colors[i+1] = RGB(random(10, 40), 0, random(45, 63) * ClydeV2_LaughBehavior::colors[i+1].r / 10); //Generate a random shade of magento
    ClydeV2_LaughBehavior::intervals[i+1] = random(50, 100); //Generate an interval value between 50 and 100 ms
    
  }

  ClydeV2_LaughBehavior::steps += random(0, 2) * 2;
  for(; i < ClydeV2_LaughBehavior::steps; i+=2) {
    ClydeV2_LaughBehavior::colors[i] = RGB(random(180, 255), random(80, 130), 0);
    ClydeV2_LaughBehavior::intervals[i] = random(300, 350);
    
    ClydeV2_LaughBehavior::colors[i+1] = RGB(random(10, 40), 0, random(45, 63) * ClydeV2_LaughBehavior::colors[i+1].r / 10);
    ClydeV2_LaughBehavior::intervals[i+1] = random(150, 200);
  }

  if (ClydeV2_LaughBehavior::steps < ClydeV2_ColorCycle::MAX_CYCLE_LENGTH)
    ClydeV2_LaughBehavior::steps++;
  
  ClydeV2_LaughBehavior::colors[ClydeV2_LaughBehavior::steps-1] = RGB(c->ambientLight.color.r, c->ambientLight.color.g, c->ambientLight.color.b);
  ClydeV2_LaughBehavior::intervals[ClydeV2_LaughBehavior::steps-1] = random(150, 200);

  c->ambientLight.colorCycle.setCycle(ClydeV2_LaughBehavior::steps, ClydeV2_LaughBehavior::colors, ClydeV2_LaughBehavior::intervals, NO_LOOP);

  //Get color light cycle duration
  _instance->cycleDuration = c->ambientLight.colorCycle.getDuration();

 #ifdef ENABLE_CHATTERBOX
    //Play the laugh track
    uint32_t songDuration = _instance->cycleDuration = c->chatterbox.playLaugh();

    //If the song duration is longer than the color cycle, change it to the song duration.
    if( songDuration  > _instance->cycleDuration ){
      _instance->cycleDuration =  songDuration;
    }
  #endif
 
}

void ClydeV2_LaughBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.print("LaughBehavior::update starttime: ");
    Serial.print(_instance->startTime);
    Serial.print("  ");
    Serial.print(millis());
    Serial.print(_instance->cycleDuration);
  #endif

  //Check if the Behavior is complete
  if( _instance->isComplete() ){
    //Behavior has come to completion
    end(c);
  }
}

void ClydeV2_LaughBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("LaughBehavior::end");
  #endif
  
  #ifdef ENABLE_CHATTERBOX
    //Stop audio
    c->chatterbox.stop();
  #endif

  //Return to default behvavior
  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
  
}


/********************************************
  * Daisy Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_DaisyBehavior::_instance = 0;

const uint8_t ClydeV2_DaisyBehavior::STEPS = 9;
const RGB ClydeV2_DaisyBehavior::COLORS[] =  {RGB(0, 255, 0), RGB(255, 0, 0), RGB(0, 0, 255),RGB(0, 255, 0), RGB(255, 0, 0), RGB(0, 0, 255), RGB(0, 255, 0), RGB(255, 0, 0), RGB(0, 0, 255)};
//const uint32_t ClydeV2_DaisyBehavior::INTERVALS[] = {3000, 400000, 300000, 200000, 180000, 120000, 3000, 2000, 1000}; //real
const uint32_t ClydeV2_DaisyBehavior::INTERVALS[] = {500, 10000, 5000, 5000, 3000, 2000, 1500, 1000, 8000}; //demo

ClydeV2_BehaviorState* ClydeV2_DaisyBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_DaisyBehavior;
  }

Serial.println(_instance->startTime);

  return _instance;
}

void ClydeV2_DaisyBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DaisyBehavior::start");
  #endif

  //Set color light cycle to daisy
  c->ambientLight.colorCycle.setCycle(ClydeV2_DaisyBehavior::STEPS, ClydeV2_DaisyBehavior::COLORS, ClydeV2_DaisyBehavior::INTERVALS, NO_LOOP);

  _instance->cycleDuration = c->ambientLight.colorCycle.getDuration();

  #ifdef ENABLE_CHATTERBOX
    //Play Daisy Bell
    uint32_t songDuration = c->chatterbox.playDaisy();

    //If the song duration is longer than the color cycle, change it to the song duration.
    if( songDuration > _instance->cycleDuration ){
      _instance->cycleDuration = songDuration;
    }

  #endif
}

void ClydeV2_DaisyBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DaisyBehavior::update");
  #endif

  //Check if the Behavior is complete
  if( _instance->isComplete() ){
    //Behavior has come to completion
    end(c);
  }
}

void ClydeV2_DaisyBehavior::interrupt(ClydeV2*) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("DaisyBehavior::interrupt");
  #endif

  //This method defines Clyde's behavior when the SquishyEye is pressed.

  //Speed up the daisy cycle
  // c->ambientLight.colorCycle.speedUp(100);

}

void ClydeV2_DaisyBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
  Serial.println("DaisyBehavior::end");
  #endif

  //Turn of color cycle
  c->ambientLight.colorCycle.stop();

  //Save the current color
  c->ambientLight.save();
  
  #ifdef ENABLE_CHATTERBOX
    //Stop audio
    c->chatterbox.stop();
  #endif

  //Return to default behvavior
  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}


/**********************************************
  * Sunset Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_SunsetBehavior::_instance = 0;

const uint8_t ClydeV2_SunsetBehavior::STEPS = 9;
const RGB ClydeV2_SunsetBehavior::COLORS[] =  {RGB(255, 150, 0), RGB(255, 0, 0), RGB(100, 0, 200), RGB(0, 0, 255), RGB(0, 0, 0), RGB(0, 0, 180), RGB(0, 0, 0), RGB(0, 0, 125), RGB(0, 0, 0)};
const uint32_t ClydeV2_SunsetBehavior::INTERVALS[] = {3000, 400000, 300000, 200000, 180000, 120000, 3000, 2000, 1000}; //real
//const uint32_t ClydeV2_SunsetBehavior::INTERVALS[] = {500, 10000, 5000, 5000, 3000, 2000, 1500, 1000, 1000}; //demo

ClydeV2_BehaviorState* ClydeV2_SunsetBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_SunsetBehavior;
  }
  return _instance;
}

void ClydeV2_SunsetBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunsetBehavior::start");
  #endif

  //Set color light cycle to sunset.
  c->ambientLight.colorCycle.setCycle(ClydeV2_SunsetBehavior::STEPS, ClydeV2_SunsetBehavior::COLORS, ClydeV2_SunsetBehavior::INTERVALS, NO_LOOP);
  _instance->cycleDuration = c->ambientLight.colorCycle.getDuration();

  #ifdef ENABLE_CHATTERBOX
    //Play lullaby
    uint32_t songDuration = c->chatterbox.playLullaby();
    Serial.println(songDuration);

    //If the song duration is longer than the color cycle, change it to the song duration.
    if(songDuration>_instance->cycleDuration){
      Serial.println("change");
      _instance->cycleDuration =  songDuration;
    }
  #endif
}

void ClydeV2_SunsetBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunsetBehavior::update");
  #endif

  // If the sunset cycle is complete, return to default behavior
  // Check if the Behavior is complete
  if( _instance->isComplete() ){
    //Behavior has come to completion
    end(c);
  }

}

void ClydeV2_SunsetBehavior::interrupt(ClydeV2* c) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunsetBehavior::interrupt");
  #endif

  //This method defines Clyde's behavior when the SquishyEye is pressed.
  end(c);
}

void ClydeV2_SunsetBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
  Serial.println("SunsetBehavior::end");
  #endif

  //Speed up the sunset cycle
  c->ambientLight.colorCycle.speedUp(100);

  #ifdef ENABLE_CHATTERBOX
    //Stop audio
    c->chatterbox.stop();
  #endif

  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}


/********************************************
  * Sunrise Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_SunriseBehavior::_instance = 0;

const uint8_t ClydeV2_SunriseBehavior::STEPS = 9;
const RGB ClydeV2_SunriseBehavior::COLORS[] =  {RGB(255, 150, 0), RGB(255, 0, 0), RGB(100, 0, 200), RGB(0, 0, 255), RGB(0, 0, 0), RGB(0, 0, 180), RGB(0, 0, 0), RGB(0, 0, 125), RGB(0, 0, 0)};
//const uint32_t SunriseBehavior::INTERVALS[] = {3000, 400000, 300000, 200000, 180000, 120000, 3000, 2000, 1000}; //real
const uint32_t ClydeV2_SunriseBehavior::INTERVALS[] = {500, 10000, 5000, 5000, 3000, 2000, 1500, 1000, 1000}; //demo

ClydeV2_BehaviorState* ClydeV2_SunriseBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_SunriseBehavior;
  }
  return _instance;
}

void ClydeV2_SunriseBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunriseBehavior::start");
  #endif

  //Set color light cycle to sunrise.
  c->ambientLight.colorCycle.setCycle(ClydeV2_SunriseBehavior::STEPS, ClydeV2_SunriseBehavior::COLORS, ClydeV2_SunriseBehavior::INTERVALS, NO_LOOP);

  #ifdef ENABLE_CHATTERBOX
    //Play lullaby
    _instance->cycleDuration = c->chatterbox.playLullaby();
  #endif
}

void ClydeV2_SunriseBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunriseBehavior::update");
  #endif

  // If the sunrise cycle is complete, return to default behavior
  // Check if the Behavior is complete
  if( _instance->isComplete() ){
    // Behavior has come to completion
    end(c);
  }
}

void ClydeV2_SunriseBehavior::interrupt(ClydeV2*) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("SunriseBehavior::interrupt");
  #endif

  //This method defines Clyde's behavior when the SquishyEye is pressed.

}

void ClydeV2_SunriseBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
  Serial.println("SunriseBehavior::end");
  #endif

  //Speed up the sunrise cycle
  c->ambientLight.colorCycle.speedUp(100);

  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}



/********************************************
  * Hypno Behavior
  **/

ClydeV2_BehaviorState* ClydeV2_HypnoBehavior::_instance = 0;

const uint8_t ClydeV2_HypnoBehavior::STEPS = 9;
const RGB ClydeV2_HypnoBehavior::COLORS[] =  {RGB(255, 150, 0), RGB(255, 0, 0), RGB(100, 0, 200), RGB(0, 0, 255), RGB(0, 0, 0), RGB(0, 0, 180), RGB(0, 0, 0), RGB(0, 0, 125), RGB(0, 0, 0)};
//const uint32_t ClydeV2_HypnoBehavior::INTERVALS[] = {3000, 400000, 300000, 200000, 180000, 120000, 3000, 2000, 1000}; //real
const uint32_t ClydeV2_HypnoBehavior::INTERVALS[] = {500, 10000, 5000, 5000, 3000, 2000, 1500, 1000, 1000}; //demo

ClydeV2_BehaviorState* ClydeV2_HypnoBehavior::instance(){
  if(_instance == 0){
    _instance = new ClydeV2_HypnoBehavior;
  }
  return _instance;
}

void ClydeV2_HypnoBehavior::start(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("HypnoBehavior::start");
  #endif

  //Set color light cycle to hypno
  c->ambientLight.colorCycle.setCycle(ClydeV2_HypnoBehavior::STEPS, ClydeV2_HypnoBehavior::COLORS, ClydeV2_HypnoBehavior::INTERVALS, NO_LOOP);

  #ifdef ENABLE_CHATTERBOX
    //Play lullaby
    _instance->cycleDuration = c->chatterbox.playLullaby();
  #endif

}

void ClydeV2_HypnoBehavior::update(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("HypnoBehavior::update");
  #endif

  //If the hypno cycle is complete, return to default behavior
  // Check if the Behavior is complete
  if( _instance->isComplete() ){
    // Behavior has come to completion
    end(c);
  }
}

void ClydeV2_HypnoBehavior::interrupt(ClydeV2*) {

  #ifdef CLYDE_DEBUG_BEHAVE
    Serial.println("HypnoBehavior::interrupt");
  #endif

  //This method defines Clyde's behavior when the SquishyEye is pressed.

}

void ClydeV2_HypnoBehavior::end(ClydeV2* c){

  #ifdef CLYDE_DEBUG_BEHAVE
  Serial.println("HypnoBehavior::end");
  #endif

  //Speed up the hypno cycle
  c->ambientLight.colorCycle.speedUp(100);

  changeBehavior(c, ClydeV2_DefaultBehavior::instance());
}

