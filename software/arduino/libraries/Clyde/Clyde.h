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
#include <SoftwareSerial.h>

#define ENABLE_AFRAID_OF_THE_DARK
#define ENABLE_TOUCHY_FEELY
//#define ENABLE_MOUTH
//#define ENABLE_EYE   // enable/disable the eye... can be buggy on some devices. Note: if not ENABLE_EYE then we have to rely on the touchyfeely module!

#include "colortypes.h"
#include "ClydeEEPROM.h"
#include "ClydeModule.h"

//#define CLYDE_DEBUG
//#define CLYDE_DEBUG_WAIT     //wait five seconds before setup/init
//#define CLYDE_DEBUG_EYE  //output extra values about the eye
//#define CLYDE_DEBUG_AFRAID   //output extra values about AoD module

/**
 * Enum types of ambient color cycles.
 */
enum ECycleType {
	OFF,
	BLINK,
	SUNSET,
	SUNRISE,
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

#ifdef ENABLE_MOUTH
/**
 * Enum types for mp3 op codes.
 */
enum EOpCode {
  //TODO check WT5001 for other useful features to implement if space
  OP_NONE = 0,
  OP_PLAY = 0xA0,
  OP_PAUSE = 0xA3,
  OP_STOP = 0xA4,
  OP_SET_VOLUME = 0xA7,
  OP_SET_PLAY_MODE = 0xA9,
  OP_SET_DATE = 0xB1,
  OP_SET_TIME = 0xB2,
  OP_PLAY_STATE = 0xC2
};

/**
 * Enum types for mp3 play modes.
 */
enum EPlayMode {
  PLAYMODE_SINGLE = 0x00,
  PLAYMODE_SINGLE_CYCLE = 0x01,
  PLAYMODE_ALL_CYCLE = 0x02,
  PLAYMODE_RANDOM = 0x03
};

/**
 * Enum types of the sound file indexes for Loudmouth
 */
enum EAudioIndex {
  SND_ON = 1,
  SND_OFF = 2,
  SND_HAPPY = 3,
  SND_LAUGH = 4,
  SND_ERROR = 5,
  SND_CLOCK_1H = 6,
  SND_CLOCK_15MIN = 7,
  SND_CLOCK_30MIN = 8,
  SND_CLOCK_45MIN = 9,
  SND_CLOCK_CHIME = 10,
  SND_NOTIFICATION = 11,
  SND_AU_CLAIR_DE_LA_LUNE = 12,
  SND_DAISY_BELL = 13
};
#endif
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
    CClydeModule* module;                        /**< Identified module. NULL if none. */
    uint8_t dpin;                                /**< Digital pin of this module's position. */
    uint8_t apin;                                /**< Analog pin of this module's position. */
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
    RGBf color;                           /**< Current color. */
    RGB targetColor;                      /**< Target color, used for fading. */
    RGB savedColor;                       /**< Saved ambient color to go back to. */
    RGBf fadeSpeed;                       /**< Speed per color channel, used for fading. */
    
    /**
     * Check if the ambient light is on.
     */
    bool isOn() { return targetColor.r > 0 || targetColor.g > 0 || targetColor.b > 0; }
    
    /** Save the current color. */
    void save() { if (color.r + color.g + color.b > 10) savedColor = RGB(color.r, color.g, color.b); }
  };
  
  /**
   * The white desk light.
   */
  struct CWhiteLight {
    uint8_t pin;            /**< Digital pin to control the brightness. */
    float brightness;       /**< Current brightness. */
    uint8_t targetBrightness; /**< Target brightness, used for fading. */
    float fadeSpeed;        /**< Speed, used for fading. */
    
    /**
     * Check if the white light is on.
     */
    bool isOn() { return targetBrightness < 255; }
  };

#ifdef ENABLE_EYE
  /**
   * The squishy eye.
   */
  struct CEye {
    //static const uint16_t CALIB_BLINK_INTERVAL = 3000;  /**< Time between white light blinks during first calibration in millis. */
    //static const uint16_t CALIB_BLINK_DURATION = 100;   /**< Time of blinks during first calibration in millis. */
    static const uint8_t CALIB_SAMPLE_INTERVAL = 10;    /**< Time between calibration samples in millis. */
    static const uint8_t CALIB_MAX_CHANGE = 40;         /**< Maximum value range accepted for calibration. */
    static const uint8_t CALIB_NUM_REPEATS = 125;       /**< Numbers of samples to check for calibration. */
    static const uint16_t CALIB_IR_BASE = 610;          /**< Base IR reading without any IR from outside. */
    static const float CALIB_FORMULA_A = 0.5;           /**< Multiplier for calibration formula. */
    static const uint16_t CALIB_FORMULA_B = 450;        /**< Base for calibration formula. */
    static const uint16_t CALIB_MIN_THRESHOLD_DIFF = 50; /**< Minimum difference between base and threshold. */    
    static const uint16_t CALIB_MAX_IR = 100;           /**< Maximum IR needed for calibration. Anything more means too much noise/sun. */
    static const uint8_t PRESS_COUNT_THRESHOLD = 8;     /**< Number of press detection needed to trigger a press event. */
    
    uint8_t pin;              /**< Analog signal pin of the IR sensor. */
    bool onceCalibrated;      /**< Flag to track the first time we calibrate. */
    bool calibrated;          /**< Flag to track if the eye is currently calibrated. */
    //bool calibBlink;          /**< Blink status during the first calibration. On/Off. */
    //uint32_t nextCalibBlink;  /**< Next time to change the first calibration blink status in millis. */
    uint32_t calibLock;       /**< Time until when the calibration can't not happen. */
    uint8_t calibCount;       /**< Number of time we read a calibration value within range in a row. */
    uint16_t irMin;           /**< Minimum sensor value read. */
    uint16_t irMax;           /**< Maximum sensor value read. */
    uint16_t irThreshold;     /**< Current threshold. */
    uint16_t irLast;          /**< Last read sensor value. */
    uint32_t pressedLast;     /**< Time when we last detected a pressed state. */
    uint32_t pressedStart;    /**< Time when we detected the start of a press. */
    uint8_t pressedCount;     /**< Number of time we detected a pressed state consecutively. */
    uint32_t pressLock;       /**< Time until when pressed events can not trigger. */
    
    #ifdef CLYDE_DEBUG
      uint16_t restartCount;  /**< Number of time calibration restarted because of noise since last calibration. */
    #endif    
  };
#endif
  
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

#ifdef ENABLE_MOUTH
  /**
   * The mouth / speaker / mp3 player
   */
  struct CMouth {
    static const uint16_t ACK_TIMEOUT = 1000;

    static const uint8_t SELECT_PIN = 4;
    static const uint8_t DETECT_PIN = 15;
    static const uint8_t RX_PIN = 14;
    static const uint8_t TX_PIN = 16;

    bool detected;
    EOpCode waitingOpCode;
    uint32_t lastCmdTime;
    
    static SoftwareSerial mp3;
  };
#endif
    
private:
  CModulePosition m_modules[CModulePosition::NUM_MODULES];
  CAmbientLight m_ambient;
  CWhiteLight m_white;
  CClydeEEPROM m_eeprom;
#ifdef ENABLE_EYE
  CEye m_eye;
#endif
  CAmbientCycle m_cycle;
#ifdef ENABLE_MOUTH
  CMouth m_mouth;
#endif
public:
  /** Contructor. */
  CClyde();
  
  /** Initialize Clyde. */
  void begin();

#ifdef ENABLE_EYE
  /** Check if eye was calibrated once. */
  bool wasEyeCalibratedOnce() { return m_eye.onceCalibrated; }

  /** Update the eye / infrared switch. */
  void updateEye();

#endif
#ifdef ENABLE_MOUTH
  /** Update the mouth / sound shield. */
  void updateMouth();
#endif
  
  /** Update the ambient light. */
  void updateAmbientLight();

  /** Update the white light. */
  void updateWhiteLight();

  /** Update the personality modules. */
  void updatePersonalities();
  
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
  
  /* /\** */
  /*  * Fade the ambient color to a given color. */
  /*  *\/ */
  /* void fadeAmbient(const RGB &c, float spd); */

  /**
   * Fade the ambient color to a given color. Higher values of tm will cause slower fading
   */
  void fadeAmbient(const RGB &c, uint8_t tm);

  /**
   * Get the white light object.
   */
  CWhiteLight* white() { return &m_white; }
  
  /**
   * Set the white light.
   */
  void setWhite(uint8_t b);
  
  /**
   * Fade the white light to a given brightness. Slower fading for higher tm values
   */
  //  void fadeWhite(uint8_t b, float spd);
  void fadeWhite(uint8_t b, uint16_t tm);

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
   * Move the cycle to a given step.
   */
  void setCycleStep(uint8_t step);
  
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
    void blink(const RGB& rgb, uint32_t onDuration, uint32_t offDuration, uint8_t numBlinks);

#ifdef ENABLE_MOUTH
  /**
   * Set the loudmouth mp3 player play mode.
   */
  EOpCode setPlayMode(EPlayMode playmode);
  
  /**
   * Play a file using the Loudmouth shield.
   */
  EOpCode play(uint16_t index);
  
  /**
   * Get the play state of the Loudmouth shield.
   */
  EOpCode playState();
  
  /**
   * Set volume of the Loudmouth shield.
   * @volume level 0 - 31
   */
  EOpCode setVolume(uint8_t volume);  
  
  /**
   * Pause the audio of the Loudmouth shield.
   */
  EOpCode pause(void);

  /**
   * Stop the audio of the Loudmouth shield.
   */
  EOpCode stop(void);
#endif
  
private:
  /** Detect the personality modules. */
  void detectPersonalities();

  /** Detect the loudmouth shield. */
  void detectMouth();

#ifdef ENABLE_EYE
  /**
   * Calibrate the eye.
   */
  void calibrateEye(uint16_t irValue);
  
  /**
   * Check if the eye was pressed given a read sensor value.
   */
  bool wasEyePressed(uint16_t irValue);
#endif
  
  /** Update a color channel of the ambient light. */
  void updateAmbientLight(float *value, uint8_t target, float speed);
  
  /** Show the current ambient light color. */
  void showAmbientLight();
    
  /** Show the current white light brightness. */
  void showWhiteLight();

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
};

extern CClyde Clyde; 

#endif
