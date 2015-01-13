/*
Clyde Behavior States
Uses State pattern.
Clyde's behavior is divided into states.

Gets a instance of Clyde passed to it, so it can
manipulate the lights and sounds.

Clyde has a few default states.  These states can be triggered
by various events.  Clyde maintains a BehaviorState object.
*/ 
#ifndef __CLYDE_BSTATES_H
#define __CLYDE_BSTATES_H

#include "ClydeV2.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/**
 * The BehaviorState
 * Defines interface to behavior states.
 */
class ClydeV2;
class ClydeV2_BehaviorState {

public:
  ClydeV2_BehaviorState();
  virtual void start(ClydeV2*);   //Start the state
  virtual void update(ClydeV2*);   //method called in main loop
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);      //End the state return to default state
  virtual void configBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks);
  bool isComplete();
  bool isDefault;
  uint32_t cycleDuration;
  uint32_t startTime;
protected:
  void changeBehavior(ClydeV2*, ClydeV2_BehaviorState*);
  
};

/**
  * Default Behavior
  **/
class ClydeV2_DefaultBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_DefaultBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);
private:
  static ClydeV2_BehaviorState* _instance;
}; 

/**
  * Blink Behavior
  **/
class ClydeV2_BlinkBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_BlinkBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void end(ClydeV2*);
  virtual void configBlink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks);
  
private:
  static ClydeV2_BehaviorState* _instance;
  static RGB rgb; 
  static uint32_t onDuration; 
  static uint32_t offDuration; 
  static uint8_t numBlinks; //Loop
};


/**
  * Select Behavior
  **/
class ClydeV2_SelectBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_SelectBehavior() : lastStopStep(0) {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void end(ClydeV2*);

  uint8_t lastStopStep;                 // step index when the color select cycle was stopped.
  static const uint8_t STEPS;           // steps in the color select cycle
  static const RGB COLORS[];            //colors of the color select cycle
  static const uint16_t INTERVALS[];    // intervals of the color select cycle

private:
  static ClydeV2_BehaviorState* _instance;
};


/**
  * Laugh Behavior / Tickle
  **/
class ClydeV2_LaughBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_LaughBehavior() : steps(0) {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void end(ClydeV2*);

private:
  static ClydeV2_BehaviorState* _instance;

  uint8_t steps; // steps in the cycle
  RGB colors[ClydeV2_ColorCycle::MAX_CYCLE_LENGTH];           //colors of the cycle
  uint16_t intervals[ClydeV2_ColorCycle::MAX_CYCLE_LENGTH];   // intervals of the cycle
};

/**
  * Daisy Behavior
  **/
class ClydeV2_DaisyBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_DaisyBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);  

  static const uint8_t STEPS;           // steps in the color select cycle
  static const RGB COLORS[];            //colors of the color select cycle
  static const uint32_t INTERVALS[];    // intervals of the color select cycle
   
private:
  static ClydeV2_BehaviorState* _instance; 
};

/**
  * Sunset Behavior
  **/
class ClydeV2_SunsetBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_SunsetBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);  

  static const uint8_t STEPS;           // steps in the color select cycle
  static const RGB COLORS[];            //colors of the color select cycle
  static const uint32_t INTERVALS[];    // intervals of the color select cycle
   
private:
  static ClydeV2_BehaviorState* _instance; 
};

/**
  * Sunrise Behavior
  **/
class ClydeV2_SunriseBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_SunriseBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);  

  static const uint8_t STEPS;           // steps in the color select cycle
  static const RGB COLORS[];            //colors of the color select cycle
  static const uint32_t INTERVALS[];    // intervals of the color select cycle
   
private:
  static ClydeV2_BehaviorState* _instance; 
};


/**
  * Hypno Behavior
  **/
class ClydeV2_HypnoBehavior : public ClydeV2_BehaviorState {
public:
  ClydeV2_HypnoBehavior() {}

  static ClydeV2_BehaviorState* instance();

  virtual void start(ClydeV2*);
  virtual void update(ClydeV2*);
  virtual void interrupt(ClydeV2*);
  virtual void end(ClydeV2*);  

  static const uint8_t STEPS;           // steps in the color select cycle
  static const RGB COLORS[];            //colors of the color select cycle
  static const uint32_t INTERVALS[];    // intervals of the color select cycle
   
private:
  static ClydeV2_BehaviorState* _instance; 
};


#endif