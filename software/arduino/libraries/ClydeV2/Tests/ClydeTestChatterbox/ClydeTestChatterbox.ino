#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SPI.h>
#include <MPR121.h>

//Test Chatterbox playback functions

#define FIRMWARE_VERSION 2

void setup(){
  
  Wire.begin();
  Clyde.begin();

  Serial.begin(9600); // start serial for debugging output
  delay(2000);
  Serial.println("Clyde is Ready!");
  Serial.println("Clyde Chatterbox");
  Serial.println();
  
  Clyde.chatterbox.playOn();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playOff();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
    
  Clyde.chatterbox.playHappy();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playLaugh();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playError();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playClock1h();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playClock15m();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playClock30m();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playClock45m();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playClockChime();
  delay(2000);
  Clyde.chatterbox.pause();
 // Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playNotification();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playLullaby();
  delay(2000);
  Clyde.chatterbox.pause();
  //Clyde.chatterbox.stop();
  
  Clyde.chatterbox.playDaisy();
  delay(2000);
  Clyde.chatterbox.pause();
  
 //Clyde.chatterbox.stop();
 //Clyde.chatterbox.pause();
}

void loop() {
}
