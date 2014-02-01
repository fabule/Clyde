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

#ifndef __CLYDE_H
#define __CLYDE_H

#include <stdint.h>

#include "colortypes.h"
#include "ClydeEEPROM.h"
#include "ClydeModule.h"

//not implemented yet
//#define CLYDE_DEBUG

/**
 * Enum types of ambient color cycles.
 */
enum ECycleType {
	OFF,
	BLINK,
	SUNSET,
  SELECT,
  LAUGH,
  UNKNOWN
};

/**
 * Enum types for cycle loop options.
 */
enum ECycleLoop {
  LOOP,
  LOOP_ONCE,
  NO_LOOP
};

/**
 * Main Clyde class that provides the interface to the device.
 */
class CClyde {
public:
  /**
   * A module position on the main Clyde controller board.
   * There are two module positions on the board.
   */
  struct CModulePosition {
    static const uint8_t NUM_MODULES = 2;        /**< Total number of module positions. */
    static const uint8_t ID_REPEAT = 8;          /**< Number of success id required to identify a module. */ //TODO adjust this
    static const uint16_t ID_INTERVAL_MS = 5000; /**< Internal in millis between module identifications. */ //TODO adjust this too
    CClydeModule* module;                        /**< Identified module. NULL if none. */
    uint8_t dpin;                                /**< Digital pin of this module's position. */
    uint8_t apin;                                /**< Analog pin of this module's position. */
    uint32_t idNext;                             /**< Time when the next identification will occur in millis. */
    CClydeModule* idLast;                        /**< Module identified last time we checked. */
    uint8_t idCount;                             /**< Number of times idLast was identified in a row. */
  };
  
  /**
   * The ambient RGB light.
   */
  struct CAmbientLight {
    static const float SCALE_CONSTRAINT;  /**< Intensity constraint because of conflict with eye's IR sensor. */
    uint8_t r_pin;                        /**< Digital pin to control red color. */
    uint8_t g_pin;                        /**< Digital pin to control green color. */
    uint8_t b_pin;                        /**< Digital pin to control blue color. */
    RGBf color;                          /**< Current color. */
    RGB targetColor;                     /**< Target color, used for fading. */
    RGB savedColor;                      /**< Saved ambient color to go back to. */
    RGBf fadeSpeed;                      /**< Speed per color channel, used for fading. */
    
    /**
     * Check if the ambient light is on.
     */
    bool isOn() { return targetColor.r > 0 || targetColor.g > 0 || targetColor.b > 0; }
    
    /** Save the current color. */
    void save() { savedColor = RGB(color.r, color.g, color.b); }
  };
  
  /**
   * The white desk light.
   */
  struct CWhiteLight {
    uint8_t pin;            /**< Digital pin to control the brightness. */
    float brightness;       /**< Current brightness. */
    float targetBrightness; /**< Target brightness, used for fading. */
    float fadeSpeed;        /**< Speed, used for fading. */
    
    /**
     * Check if the white light is on.
     */
    bool isOn() { return targetBrightness < 255; }
  };
  
  /**
   * The squishy eye.
   */
  struct CEye {
    static const uint16_t CALIB_BLINK_INTERVAL = 3000;  /**< Time between white light blinks during first calibration in millis. */
    static const uint16_t CALIB_BLINK_DURATION = 100;   /**< Time of blinks during first calibration in millis. */
    static const uint8_t CALIB_SAMPLE_INTERVAL = 10;    /**< Time between calibration samples in millis. */
    static const uint8_t CALIB_MAX_CHANGE = 40;         /**< Maximum value range accepted for calibration. */
    static const uint8_t CALIB_NUM_REPEATS = 125;       /**< Numbers of samples to check for calibration. */
    static const uint16_t CALIB_MIN_THRESHOLD = 500;    /**< Minimum calibration threshold. Anything below means too much noise. */
    static const uint8_t PRESS_COUNT_THRESHOLD = 8;     /**< Number of press detection needed to trigger a press event. */
    
    uint8_t pin;              /**< Analog signal pin of the IR sensor. */
    bool onceCalibrated;      /**< Flag to track the first time we calibrate. */
    bool calibrated;          /**< Flag to track if the eye is currently calibrated. */
    bool calibBlink;          /**< Blink status during the first calibration. On/Off. */
    uint32_t nextCalibBlink;  /**< Next time to change the first calibration blink status in millis. */
    uint8_t calibCount;       /**< Number of time we read a calibration value within range in a row. */
    uint16_t irMin;           /**< Minimum sensor value read. */
    uint16_t irMax;           /**< Maximum sensor value read. */
    uint16_t irThreshold;     /**< Current threshold. */
    uint16_t irLast;          /**< Last read sensor value. */
    uint32_t pressedLast;     /**< Time when we last detected a pressed state. */
    uint32_t pressedStart;    /**< Time when we detected the start of a press. */
    uint8_t pressedCount;     /**< Number of time we detected a pressed state consecutively. */
  };
  
  /**
   * The ambient light cycle.
   */
  struct CAmbientCycle {
    static const uint8_t MAX_CYCLE_LENGTH = 12; /**< Maximum cycle steps. */
    ECycleType type;                            /**< Type of the current cycle. */
    uint8_t numSteps;                           /**< Number of steps. */
    uint8_t step;                               /**< Current step. */
    uint32_t stepStart;                         /**< Time when the current step started in millis. */
    uint32_t stepEnd;                           /**< Time when the current step end in millis. */
    RGB stepColor;                              /**< Start color of the current step. */
    RGB colors[MAX_CYCLE_LENGTH];               /**< Colors of cycle. */
    uint32_t intervals[MAX_CYCLE_LENGTH];       /**< Intervals in millis. */
    ECycleLoop loop;                            /**< Looping parameter. */
    
    /**
     * Check if there is an ongoing cycle.
     */
    bool isOn() { return type != OFF; }
    
    /**
     * Check if the current cycle matches a given type.
     */
    bool is(ECycleType t) { return t == type; }
    
    /** Turn off the cycle. */
    void off() { type = OFF; }
  };
    
private:
  CModulePosition m_modules[CModulePosition::NUM_MODULES];
  CAmbientLight m_ambient;
  CWhiteLight m_white;
  CClydeEEPROM m_eeprom;
  CEye m_eye;
  CAmbientCycle m_cycle;
  
public:
  /** Contructor. */
  CClyde();
  
  /** Initialize Clyde. */
  void begin();
  
  /** Update Clyde each loop. */
  void update();
  
  /**
   * Get a module position for a given index.
   */
  CModulePosition* module(uint8_t m) { return &m_modules[m]; }
  
  /**
   * Get the eeprom object.
   */
  CClydeEEPROM* eeprom() { return &m_eeprom; }

  /**
   * Get the ambient light object.
   */
  CAmbientLight* ambient() { return &m_ambient; }
  
  /**
   * Set ambient color.
   */
  void setAmbient(const RGB &c);
  
  /**
   * Fade the ambient color to a given color.
   */
  void fadeAmbient(const RGB &c, float spd);
    
  /**
   * Get the white light object.
   */
  CWhiteLight* white() { return &m_white; }
  
  /**
   * Set the white light.
   */
  void setWhite(uint8_t b);
  
  /**
   * Fade the white light to a given brightness.
   */
  void fadeWhite(uint8_t b, float spd);

  /** Switch to the next of the four lights on/off states. */
  void switchLights();
  
  /**
   * Get the ambient light cycle object.
   */
  CAmbientCycle* cycle() { return &m_cycle; }
  
  /**
   * Set the ambient light cycle.
   */
  void setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint8_t *intervals, ECycleLoop loop);
  
  /**
   * Set the ambient light cycle.
   */
  void setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint16_t *intervals, ECycleLoop loop);

  /**
   * Set the ambient light cycle.
   */
  void setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint32_t *intervals, ECycleLoop loop);

  /**
   * Skip to the next cycle step.
   */
  void cycleNextStep(uint32_t now);
  
  /** Speed up the end of the current cycle. */
  void speedUpCycle(uint32_t factor);

  /** Stop the cycle. */
  void stopCycle();
  
  /**
   * Make the ambient light blink.
   */
  void blink(const RGB& rgb, uint8_t numBlinks, uint8_t msInterval);

private:
  /** Update the eye. */
  void updateEye();

  /**
   * Calibrate the eye.
   */
  void calibrateEye(uint16_t irValue);
  
  /**
   * Check if the eye was pressed given a read sensor value.
   */
  bool wasEyePressed(uint16_t irValue);

  /** Update the ambient light. */
  void updateAmbientLight();
  
  /** Update a color channel of the ambient light. */
  void updateAmbientLight(float *value, uint8_t target, float speed);
  
  /** Show the current ambient light color. */
  void showAmbientLight();
  
  /** Update the white light. */
  void updateWhiteLight();
  
  /** Show the curent white light brightness. */
  void showWhiteLight();

  /**
   * Set the ambient light cycle.
   */
  //void setCycle(ECycleType type, uint8_t steps, const uint8_t* colors, ECycleLoop loop);
  
  /**
   * Set the ambient light cycle.
   */
  void setCycle(ECycleType type, uint8_t steps, const RGB* colors, ECycleLoop loop);
  
  /** Update the ambient light cycle. */
  void updateCycle();
  
  /**
   * Update the ambient light cycle to the next step.
   */
  void updateCycleNextStep(uint32_t now);

  /** Update the personality modules. */
  void updatePersonalities();
};

extern CClyde Clyde; 

#endif