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


/* HERE IS WHERE I START TO PIECE TOGETHER THE NEW SHIELD FIRMWARE FROM 
  THE ARDUINO VS1003 LIB, FRANTZ'S CODE AND THE NEW SPI LIBRARY */

#ifndef __CLYDE_CHATTERBOX_H
#define __CLYDE_CHATTERBOX_H
  
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ClydeV2_Chattershield.h"

  /**
 * Enum types of the sound file indexes for Chatterbox
 */
enum EChatterboxAudioIndex {
  SND_ON = 0,                   //  2000 ms
  SND_OFF = 1,                  //  2000 ms
  SND_HAPPY = 2,                // 20650 ms
  SND_LAUGH = 3,                //  9000 ms
  SND_ERROR = 4,                //   600 ms
  SND_CLOCK_1H = 5,             // 24000 ms
  SND_CLOCK_15MIN = 6,          //  6000 ms
  SND_CLOCK_30MIN = 7,          // 12000 ms
  SND_CLOCK_45MIN = 8,          // 18000 ms
  SND_CLOCK_CHIME = 9,          // 24000 ms
  SND_NOTIFICATION = 10,        //  1200 ms
  SND_AU_CLAIR_DE_LA_LUNE = 11, // 44000 ms
  SND_DAISY_BELL = 12           // 36000 ms
};

/**
 * The Chatterbox
 * VS1003
 * The mouth / speaker / mp3 player
 */
class ClydeV2_Chatterbox {

public:
  static ClydeV2_Chattershield chattershield;
  uint16_t expectedNumTracks;
  bool detected; // Has the Chattershield been detected?
  bool ready; // Is the Chattershield ready to play?  If the sheild is detected, but not ready, then there is likely an issue with the SD card.

public:  
  /** Constructor. */

ClydeV2_Chatterbox()
  :  detected(false),
  ready(false),
  expectedNumTracks(0) {}

  /** Initialize Clyde. */
  bool init(uint16_t _num_tracks);

  /* Play Clyde's sounds */
  uint16_t playOn();
  uint16_t playOff();
  uint16_t playHappy();
  uint16_t playLaugh();
  uint16_t playError();
  uint16_t playClock1h();
  uint16_t playClock15m();
  uint16_t playClock30m();
  uint16_t playClock45m();
  uint16_t playClockChime();
  uint16_t playNotification();
  uint16_t playLullaby();
  uint16_t playDaisy();

  /**
   * Set volume of the Chattershield.
   * @volume level 0 - 255
   */
  void setVolume(uint8_t volume);  
  /**  Pause the audio of the Chattershield. */
  void pause(void);
  /**  Stop the audio of the Chattershield. Fades out the current song. */
  void stop(void);

private:

  /** Verify the Chattershield contains to correct number of audio files. */
  bool verify();
  /**  Play a file using the Chattershield. */
  void play(uint16_t index);
  /**  Play a file using the Chattershield. */
  void playLoop(uint16_t index);

};

#endif