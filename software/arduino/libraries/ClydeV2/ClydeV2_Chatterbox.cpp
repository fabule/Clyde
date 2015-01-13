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

#include "ClydeV2_DebugFlags.h"
#include "ClydeV2_Chatterbox.h"
#include "ClydeV2_Chattershield.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

bool ClydeV2_Chatterbox::init(uint16_t _num_tracks) {

  #ifdef CLYDE_DEBUG_CHATTERBOX
    Serial.println("Chatterbox::init");
  #endif

  expectedNumTracks = _num_tracks;
  uint8_t detectAttemptCount = 0;
  uint8_t detectAttemptMaxCount = 3;
  bool cardVerified = false;

  // Check for the chattershield.  Check until it is ready, or until we have check
  // detectMaxCount number of times.
  while( !ready && detectAttemptCount < detectAttemptMaxCount ){

    // If this is not the first time checking, wait a little while before checking again
    if( detectAttemptCount > 0 ){
     // delay(10);
    }

    // First check if the chattershield is there.
    if(!detected){
      // Initialize the chattershield
      detected = chattershield.init();
    }

    //If the chattershield has been detected, then verify the sd card
    if(detected){
      //Make sure that the shield contains correct audio files
      cardVerified = verify();
    }

    // If the shield is detected and the sd is verfifed, then chatterbox is ready.
    ready = detected && cardVerified;
    detectAttemptCount++;
  }

  #ifdef CLYDE_DEBUG_CHATTERBOX
    Serial.print("Chatterbox detectAttemptCount: ");
    Serial.print(detectAttemptCount);
    Serial.print(" ready? ");
    Serial.print(ready);
    Serial.print(" Chattershield detected? ");
    Serial.print(detected);
    Serial.print(" SD card verified? ");
    Serial.println(cardVerified);
  #endif
}
uint16_t ClydeV2_Chatterbox::playOn() {
  play(SND_ON);
  //  2000 ms
  return 2000;
}
uint16_t ClydeV2_Chatterbox::playOff() {
  play(SND_OFF);
  //  2000 ms
  return 2000; 
}
uint16_t ClydeV2_Chatterbox::playHappy() {
  playLoop(SND_HAPPY);
  // 20650 ms
  return 20650;
}
uint16_t ClydeV2_Chatterbox::playLaugh() {
  play(SND_LAUGH);
  //  9000 ms
  return 9000;
}
uint16_t ClydeV2_Chatterbox::playError() {
  play(SND_ERROR);
  //   600 ms
  return 600;
}
uint16_t ClydeV2_Chatterbox::playClock1h() {
  play(SND_CLOCK_1H);
  // 24000 ms
  return 24000;
}
uint16_t ClydeV2_Chatterbox::playClock15m() {
  play(SND_CLOCK_15MIN);
  //  6000 ms
  return 6000;
}
uint16_t ClydeV2_Chatterbox::playClock30m() {
  play(SND_CLOCK_30MIN);
  // 12000 ms
  return 12000;
}
uint16_t ClydeV2_Chatterbox::playClock45m() {
  play(SND_CLOCK_45MIN);
  // 18000 ms
  return 18000;
}
uint16_t ClydeV2_Chatterbox::playClockChime() {
  play(SND_CLOCK_CHIME);
  // 24000 ms
  return 24000;
}
uint16_t ClydeV2_Chatterbox::playNotification() {
  play(SND_CLOCK_CHIME);
  //  1200 ms
  return 1200;
}
uint16_t ClydeV2_Chatterbox::playLullaby() {
  play(SND_AU_CLAIR_DE_LA_LUNE);
  // 44000 ms
  return 44000;
}
uint16_t ClydeV2_Chatterbox::playDaisy() {
  playLoop(SND_DAISY_BELL);
  // 36000 ms
  return 36000;
}

void ClydeV2_Chatterbox::setVolume(uint8_t volume){

  if(ready){

    #ifdef CLYDE_DEBUG_CHATTERBOX
      Serial.println("Chatterbox::setVolume");
    #endif

    //If the volume is out of range, set it to the max value
    if (volume > 255) volume = 255;
    //Invert the number, so that 0 is off and 255 is full blast
    volume =  255 - volume;

    chattershield.setVolume(volume);
  }
}

void ClydeV2_Chatterbox::pause(void){

  if(ready){

    #ifdef CLYDE_DEBUG_CHATTERBOX
      Serial.println("Chatterbox::pause");
    #endif

    chattershield.pause();
  }
}

void ClydeV2_Chatterbox::stop(void){

  if(ready){

    #ifdef CLYDE_DEBUG_CHATTERBOX
      Serial.println("Chatterbox::stop");
    #endif

    chattershield.fadeOut();
  }
}

void ClydeV2_Chatterbox::play(uint16_t index){

  if(ready){

    #ifdef CLYDE_DEBUG_CHATTERBOX
      Serial.println("Chatterbox::play");
    #endif

    //Set the volume to full blast
    chattershield.setVolume(0x00);
    //Play the track and then stop
    chattershield.playTrack(index, false);
  }
}

void ClydeV2_Chatterbox::playLoop(uint16_t index){

  if(ready){

    #ifdef CLYDE_DEBUG_CHATTERBOX
      Serial.println("Chatterbox::playLoop");
    #endif

    //Set the volume to full blast
    chattershield.setVolume(0x00);
    //Play the track in a loop
    chattershield.playTrack(index, true);
  }
}

bool ClydeV2_Chatterbox::verify() {

  #ifdef CLYDE_DEBUG_CHATTERBOX
    Serial.println("Chatterbox::verify");
  #endif

  uint16_t numTracks = chattershield.getTrackCount();
  uint16_t currentTrack = chattershield.getCurrentTrackNumber();

  #ifdef CLYDE_DEBUG_CHATTERBOX
    Serial.print("Number of tracks: ");
    Serial.print(numTracks);
    Serial.print(" currentTrack: ");
    Serial.println(currentTrack);
  #endif

  //Ensure that we have the correct number of audio tracks on the microSD card, 
  //and that the current track is 0
  //If these conditions are met, the chattershield is detected.
  if( numTracks==expectedNumTracks && currentTrack==0){
    return true;
  }else{
    return false;
  }
}