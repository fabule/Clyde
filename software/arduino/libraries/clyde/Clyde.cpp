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

#include "Clyde.h"
#include "ClydeAfraidOfTheDark.h"
#include "ClydeTouchyFeely.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

CClyde Clyde;

const float CClyde::CAmbientLight::SCALE_CONSTRAINT = 225.0f / 255.0f;

SoftwareSerial CClyde::CMouth::mp3(CClyde::CMouth::RX_PIN, CClyde::CMouth::TX_PIN);

CClyde::CClyde() {
  //init modules
  m_modules[0].module = NULL;
  m_modules[0].dpin = 7;
  m_modules[0].apin = 1;
  m_modules[0].idLast = NULL;
  m_modules[0].idCount = 0;
  m_modules[1].module = NULL;
  m_modules[1].dpin = 8;
  m_modules[1].apin = 2;
  m_modules[1].idLast = NULL;
  m_modules[1].idCount = 0;
  
  //init ambient light
  m_ambient.r_pin = 5;
  m_ambient.g_pin = 6;
  m_ambient.b_pin = 9;
  m_ambient.savedColor = RGB(0, 0, 0);
  m_ambient.fadeSpeed = RGB(0, 0, 0);
  setAmbient(RGB(0,0,0));
  
  //init white light
  //we use 254 brightness as a hack to remove the flicker on first fade-in,
  //but this adds a flash on startup instead, which is kinda neat
  //TODO look for a better solution
  m_white.pin = 11;
  setWhite(254);
  
  //init eye
  m_eye.pin = 0;
  m_eye.onceCalibrated = false;
  m_eye.calibrated = false;
  m_eye.calibBlink = true;
  m_eye.nextCalibBlink = 3000;
  m_eye.calibLock = 0;
  m_eye.calibCount = 0;
  m_eye.irMin = 1025;
  m_eye.irMax = 0;
  m_eye.irThreshold = 0;
  m_eye.irLast = 0;
  m_eye.pressedLast = 0;
  m_eye.pressedStart = 0;
  m_eye.pressedCount = 0;
  m_eye.pressLock = 0;
  
  //init ambient cycle
  m_cycle.type = OFF;
  m_cycle.numSteps = 0;
  m_cycle.step = 0;
  m_cycle.stepStart = 0;
  m_cycle.stepEnd = 0;
  m_cycle.stepColor = RGB(0, 0, 0);
  memset((void*)&m_cycle.colors[0], 0, sizeof(RGB)*CAmbientCycle::MAX_CYCLE_LENGTH);
  memset((void*)&m_cycle.intervals[0], 0, sizeof(uint32_t)*CAmbientCycle::MAX_CYCLE_LENGTH);
  m_cycle.loop = NO_LOOP;
  
  m_mouth.detected = false;
  m_mouth.waitingOpCode = OP_NONE;
  m_mouth.lastCmdTime = 0;
}

void CClyde::begin() {
  //setup module pins
  pinMode(m_modules[0].dpin, INPUT);
  digitalWrite(m_modules[0].dpin, LOW);
  pinMode(m_modules[1].dpin, INPUT);
  digitalWrite(m_modules[1].dpin, LOW);

  //setup ambient light pins
  pinMode(m_ambient.r_pin, OUTPUT);
  pinMode(m_ambient.g_pin, OUTPUT); 
  pinMode(m_ambient.b_pin, OUTPUT); 
  analogWrite(m_ambient.r_pin, m_ambient.color.r);
  analogWrite(m_ambient.g_pin, m_ambient.color.g);
  analogWrite(m_ambient.b_pin, m_ambient.color.b);

  //setup white light pins
  pinMode(m_white.pin, OUTPUT);
  analogWrite(m_white.pin, m_white.brightness);

  //setup mouth / mp3 shield
  m_mouth.mp3.begin(9600);

  pinMode(CMouth::SELECT_PIN, OUTPUT);
  pinMode(CMouth::DETECT_PIN, INPUT);
  pinMode(CMouth::RX_PIN, INPUT);
  pinMode(CMouth::TX_PIN, OUTPUT);
  
  digitalWrite(CMouth::SELECT_PIN, HIGH);
  
  //load parameters from eeprom
  m_eeprom.readAmbientColor(&m_ambient.savedColor);
  
  //detect the personality modules
  detectPersonalities();
  
  //detect the loudmouth shield
  detectMouth();
  
  //set default lights off
  setAmbient(RGB(0,0,0));
  setWhite(255);
}

void CClyde::detectPersonalities() {
  //detect personalities
  for(int i = 0; i <= CModulePosition::ID_REPEAT; i++) {
    //check each module position
    
    for(int j = 0; j < CModulePosition::NUM_MODULES; j++) {
      CClydeModule* newModule = NULL;
      
      //TODO: this could be dynamic, modules could register. could have different detection schemes.
      pinMode(m_modules[j].dpin, OUTPUT);
      digitalWrite(m_modules[j].dpin, HIGH);
      uint16_t idValue = analogRead(m_modules[j].apin);
      pinMode(m_modules[j].dpin, INPUT);
      
      //check for each type of module
      if (AfraidOfTheDark.id(idValue))
        newModule = &AfraidOfTheDark;
      else if (TouchyFeely.id(idValue))
        newModule = &TouchyFeely;
        
      //if the detected module is different that last, then reset detection count
      //TODO: reverse this if block to check for equality first
      if (newModule != m_modules[j].idLast) {
        m_modules[j].idCount = 1;
        m_modules[j].idLast = newModule;
        newModule = NULL;
      }
      //if the detected module is the same as the last, then increase detection count
      else if (m_modules[j].idCount < CModulePosition::ID_REPEAT) {
        m_modules[j].idCount++;
        newModule = NULL;
      }
      //if the detected module has been the same enough times, then init the module
      else {
        if (m_modules[j].idCount == CModulePosition::ID_REPEAT) {
          //if a module is detected, init
          if (newModule != NULL) {
            if (newModule->init(m_modules[j].apin, m_modules[j].dpin)) {
              m_modules[j].module = newModule;
              m_modules[j].idCount++;
            }
          }
          //if no module is detected, the reset module position to default
          else {     
            pinMode(m_modules[j].dpin, INPUT);
            digitalWrite(m_modules[j].dpin, LOW); 
            
            m_modules[j].module = newModule;          
            m_modules[j].idCount++; 
          }
        }
      }
    }
  }
}

void CClyde::detectMouth() {
  #ifdef CLYDE_DEBUG
    Serial.println("Clyde: Trying to detect Loudmouth. Request set play mode: Single Loop Mode");
  #endif
  m_mouth.waitingOpCode = Clyde.setPlayMode(PLAYMODE_SINGLE_CYCLE);
}

void CClyde::updateEye() {
  //read IR value
  uint16_t irValue = analogRead(m_eye.pin);

  //calibrated the eye's IR sensor
  calibrateEye(irValue);

  //if the eye was pressed
  if (wasEyePressed(irValue)) {
    #ifdef CLYDE_DEBUG
      Serial.print("Clyde: eye was pressed.");
      if (m_cycle.isOn()) Serial.println(" stopped cycle.");
      else Serial.println(" switched lights. ");
    #endif  

    //and the ambient cycle is on, then turn it off
    if (m_cycle.isOn()) 
      stopCycle();
    //if not, then switch the lights to the next state
    else
      switchLights();
  }
}

void CClyde::calibrateEye(uint16_t irValue) {
  #ifdef CLYDE_DEBUG
    static int restartCount = 0;
  #endif
  
  //check if calibration was locked
  if (millis() < m_eye.calibLock) return;
  
  //if IR has never been calibrated, blink white light until it is
  if(!m_eye.onceCalibrated) {
    if (millis() > m_eye.nextCalibBlink) {
      m_eye.calibBlink = !m_eye.calibBlink;
      setWhite(m_eye.calibBlink ? 0 : 255);
      m_eye.nextCalibBlink += m_eye.calibBlink ? CEye::CALIB_BLINK_DURATION : CEye::CALIB_BLINK_INTERVAL;
    }
  }
  
  //if the eye is pressed, don't try to calibrate
  if (m_eye.pressedCount > 0) return;
  
  //get difference since last time
  int32_t irDiff = irValue > m_eye.irLast ? irValue - m_eye.irLast : m_eye.irLast - irValue;

  //check if the IR value is stable, if so continue collecting samples
  if (irDiff < CEye::CALIB_MAX_CHANGE) {
    m_eye.calibCount++;
    m_eye.irMin = irValue < m_eye.irMin ? irValue : m_eye.irMin;
    m_eye.irMax = irValue > m_eye.irMax ? irValue : m_eye.irMax;
  }
  //if the value is too far from the previous, restart calibration
  else {
    m_eye.calibCount = 0;
    m_eye.irMin = m_eye.irMax = irValue;
    
    #ifdef CLYDE_DEBUG
      restartCount++;
    #endif
  }

  //check if we've read enough samples to calibrate
  if (m_eye.calibCount >= CEye::CALIB_NUM_REPEATS) {
    
    //check if the range of values is beyond the threshold, if so then restart
    if (m_eye.irMax-m_eye.irMin >= CEye::CALIB_MAX_CHANGE) {
      m_eye.calibCount = 0;
      m_eye.irMin = m_eye.irMax = irValue;
      return;
    }
    
    //average ir reading
    uint16_t irAvg = (m_eye.irMin + m_eye.irMax) / 2;
    
    //check to make sure that there's enough IR emitted by the circuit
    if (irAvg > (uint16_t)((CEye::CALIB_FORMULA_B - CEye::CALIB_MIN_THRESHOLD_DIFF) / CEye::CALIB_FORMULA_A)) {
      blink(RGB(255, 0, 0), 3, 200);
      setWhite(255);
      m_eye.calibrated = false;
      
      setPlayMode(PLAYMODE_SINGLE);
      play(SND_ERROR);
      
      #ifdef CLYDE_DEBUG
      Serial.print("Clyde: eye uncalibrated. not enough IR detected, check circuit. ir = ");
      Serial.print(irAvg);
      Serial.print(", minimum = ");
      Serial.println((uint16_t)((CEye::CALIB_FORMULA_B - CEye::CALIB_MIN_THRESHOLD_DIFF) / CEye::CALIB_FORMULA_A));
      #endif
    }
    //only calibrate if the threshold is above a certain limit
    //if not it's too unpredictable (e.g. the sun is shining on it)
    else {
      //if the eye was not calibrated, turn on ambient light to show feedback
      if (!m_eye.calibrated)
        fadeAmbient(m_ambient.savedColor, 0.1f);

      m_eye.calibrated = true;
      m_eye.onceCalibrated = true;

      //calculate the threshold
      //simple conversion from detected base ir to threshold
      //the less ir detected (higher value) the less difference required to trigger
      uint16_t newThreshold = irAvg * CEye::CALIB_FORMULA_A + CEye::CALIB_FORMULA_B;
/*
      #ifdef CLYDE_DEBUG
        if (m_eye.irThreshold != newThreshold) {
          Serial.print("Clyde: eye calibrated. threshold = ");
          Serial.print(newThreshold);
          Serial.print(", range = ");
          Serial.print(m_eye.irMax - m_eye.irMin);
          Serial.print(", noisy restarts = ");
          Serial.println(restartCount);
        }
        restartCount = 0;
      #endif      
*/     
      m_eye.irThreshold = newThreshold;
    }

    //reset values
    m_eye.calibCount = 0;
    m_eye.irMin = 1025;
    m_eye.irMax = 0;
  }

  m_eye.irLast = irValue;
}

bool CClyde::wasEyePressed(uint16_t irValue) {
  //require that IR is calibrated
  if (!m_eye.calibrated || millis() < m_eye.pressLock) return false;

  //if the eye press is detected enough time, trigger press event
  if (m_eye.pressedCount == CEye::PRESS_COUNT_THRESHOLD) {
    //and we detect that's it's still pressed,
    //then keep track of the last time is was detected
    if (irValue >= m_eye.irThreshold) {
      m_eye.pressedLast = millis();
      //if the eye has been pressed for a some time, auto release
      if (millis() > m_eye.pressedStart+3000) {
        m_eye.pressedCount = 0;
        m_eye.calibLock = m_eye.pressLock = millis() + 1500;
        blink(RGB(255,0,0), 3, 200);
        setWhite(255);
        
        setPlayMode(PLAYMODE_SINGLE);
        play(SND_ERROR);
      
        #ifdef CLYDE_DEBUG
        Serial.println("Clyde: eye long press detected. auto release.");
        #endif
      }
    }
    //if it's not pressed, and it's been a bit of time, then release
    else if (millis() > m_eye.pressedLast+200) {
      m_eye.pressedCount = 0;
    }
  }
  //if the eye is pressed, increase detection count
  else if (irValue >= m_eye.irThreshold) {
    m_eye.pressedCount++;
    m_eye.pressedLast = millis();
    if (m_eye.pressedCount == CEye::PRESS_COUNT_THRESHOLD) {
      m_eye.pressedStart = m_eye.pressedLast;
      return true;
    }
  }
  //if it's been some time since the last detection of a released state, then release
  else if (millis() > m_eye.pressedLast+500) {
    m_eye.pressedCount = 0;
  }

  return false;
}

void CClyde::updateMouth() {
  //detect the loudmouth shield at startup by waiting for mp3 player response
  if (!m_mouth.detected) {
    //only wait for a few seconds
    if (m_mouth.waitingOpCode == OP_NONE) {
      return;
    }
    else if (m_mouth.mp3.available() && PLAYMODE_SINGLE_CYCLE == m_mouth.mp3.read()) {
      #ifdef CLYDE_DEBUG
      Serial.println("Clyde: Loudmouth detected. Set play mode: Single Loop Mode.");
      #endif
      m_mouth.detected = true;
    }
    else if (m_mouth.lastCmdTime - millis() > CMouth::ACK_TIMEOUT) {
      #ifdef CLYDE_DEBUG
      Serial.println("Clyde: Loudmouth not detected. Set play mode timeout.");
      #endif
      m_mouth.waitingOpCode = OP_NONE;
    }
  }
  else {
    //TODO
  }
}

EOpCode CClyde::setPlayMode(EPlayMode playmode)
{
  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x03);
  m_mouth.mp3.write(OP_SET_PLAY_MODE);
  m_mouth.mp3.write(playmode);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_SET_PLAY_MODE;
}

EOpCode CClyde::play(uint16_t index)
{
  if (!m_mouth.detected) return OP_NONE;
  
  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x04);
  m_mouth.mp3.write(OP_PLAY);
  m_mouth.mp3.write((index >> 8) & 0xFF);
  m_mouth.mp3.write(index & 0xFF);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_PLAY;
}

EOpCode CClyde::playState()
{
  if (!m_mouth.detected) return OP_NONE;

  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x02);
  m_mouth.mp3.write(OP_PLAY_STATE);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_PLAY_STATE;
}

EOpCode CClyde::setVolume(uint8_t volume)
{
  if (!m_mouth.detected) return OP_NONE;

  if (volume > 31) volume = 31;

  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x03);
  m_mouth.mp3.write(OP_SET_VOLUME);
  m_mouth.mp3.write(volume);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_SET_VOLUME;
}

EOpCode CClyde::pause(void)
{
  if (!m_mouth.detected) return OP_NONE;

  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x02);
  m_mouth.mp3.write(OP_PAUSE);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_PAUSE;
}

EOpCode CClyde::stop(void)
{
  if (!m_mouth.detected) return OP_NONE;

  m_mouth.mp3.write(0x7E);
  m_mouth.mp3.write(0x02);
  m_mouth.mp3.write(OP_STOP);
  m_mouth.mp3.write(0x7E);
  
  m_mouth.lastCmdTime = millis();
  
  return OP_PAUSE;
}

void CClyde::updateAmbientLight() {
  //update ambient cycle
  if (m_cycle.isOn())
    updateCycle();

  //fade each rgb value
  updateAmbientLight(&m_ambient.color.r, m_ambient.targetColor.r, m_ambient.fadeSpeed.r);
  updateAmbientLight(&m_ambient.color.g, m_ambient.targetColor.g, m_ambient.fadeSpeed.g);
  updateAmbientLight(&m_ambient.color.b, m_ambient.targetColor.b, m_ambient.fadeSpeed.b);

  showAmbientLight();
}

void CClyde::updateAmbientLight(float *value, uint8_t target, float speed) {
  //only fade if we haven't reached the desired level
  if (target == *value)
    return;

  //get the difference to the target, and the fade direction
  float diff = target - *value;
  int8_t dir = diff < 0 ? -1 : 1;

  //fade and limit to the defined max fade speed
  *value += diff*dir < speed ? diff : speed * dir;
}

void CClyde::showAmbientLight() {
  //output new color
  analogWrite(m_ambient.r_pin, (uint8_t)(m_ambient.color.r * CAmbientLight::SCALE_CONSTRAINT));
  analogWrite(m_ambient.g_pin, (uint8_t)(m_ambient.color.g * CAmbientLight::SCALE_CONSTRAINT));
  analogWrite(m_ambient.b_pin, (uint8_t)(m_ambient.color.b * CAmbientLight::SCALE_CONSTRAINT));  
}

void CClyde::updateWhiteLight() {
  //only fade if we haven't reached the desired level
  if (m_white.targetBrightness == m_white.brightness)
    return;

  //get the difference to the target, and the fade direction
  float diff = m_white.targetBrightness - m_white.brightness;
  int8_t dir = diff < 0 ? -1 : 1;

  //fade and limit to the defined max fade speed
  m_white.brightness += diff*dir < m_white.fadeSpeed ? diff : m_white.fadeSpeed*dir;

  //output new level
  showWhiteLight();
}

void CClyde::showWhiteLight() {
  analogWrite(m_white.pin, m_white.brightness);
}

void CClyde::setAmbient(const RGB &c) {
  m_ambient.targetColor = c;
  m_ambient.color = m_ambient.targetColor;
  showAmbientLight();
}

void CClyde::updatePersonalities() {
  for(int i = 0; i < CModulePosition::NUM_MODULES; i++) {
    if (m_modules[i].module != NULL)
      m_modules[i].module->update(m_modules[i].apin, m_modules[i].dpin);
  }
}

void CClyde::fadeAmbient(const RGB &c, float spd) {
  m_ambient.targetColor = c;
  
  //calculate fade speed for each color
  m_ambient.fadeSpeed = RGBf(
    (m_ambient.targetColor.r - m_ambient.color.r) / 255.0f * spd,
    (m_ambient.targetColor.g - m_ambient.color.g) / 255.0f * spd,
    (m_ambient.targetColor.b - m_ambient.color.b) / 255.0f * spd
  );

  //make sure that fade speeds are positive
  if (m_ambient.fadeSpeed.r < 0) m_ambient.fadeSpeed.r *= -1;
  if (m_ambient.fadeSpeed.g < 0) m_ambient.fadeSpeed.g *= -1;
  if (m_ambient.fadeSpeed.b < 0) m_ambient.fadeSpeed.b *= -1;
}

void CClyde::setWhite(uint8_t b) {
  m_white.brightness = m_white.targetBrightness = b;
  showWhiteLight();
}

void CClyde::fadeWhite(uint8_t b, float spd) {
  m_white.targetBrightness = b;

  m_white.fadeSpeed = (m_white.targetBrightness - m_white.brightness) / 255.0 * spd;
  if (m_white.fadeSpeed < 0) m_white.fadeSpeed *= -1;
}

void CClyde::switchLights()
{ 
  if (!m_white.isOn() && m_ambient.isOn()) {
    fadeWhite(0, 0.1f);
  }
  else if (m_white.isOn() && m_ambient.isOn()) {
    fadeAmbient(RGB(0,0,0), 0.5f);
  }
  else if (m_white.isOn() && !m_ambient.isOn()) {
    fadeWhite(255, 0.3f);
    setPlayMode(PLAYMODE_SINGLE);
    play(SND_OFF);
  }
  else if (!m_white.isOn() && !m_ambient.isOn()) {
    fadeAmbient(m_ambient.savedColor, 0.1f);
    setPlayMode(PLAYMODE_SINGLE);
    play(SND_ON);
  }
}

void CClyde::setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint8_t *intervals, ECycleLoop loop) {
  for(int i = 0; i < steps; i++)
    m_cycle.intervals[i] = *(intervals + i);
  
  setCycle(type, steps, colors, loop);  
}
void CClyde::setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint16_t *intervals, ECycleLoop loop) {
  for(int i = 0; i < steps; i++)
    m_cycle.intervals[i] = *(intervals + i);
  
  setCycle(type, steps, colors, loop);  
}
void CClyde::setCycle(ECycleType type, uint8_t steps, const RGB *colors, const uint32_t *intervals, ECycleLoop loop) {
  for(int i = 0; i < steps; i++)
    m_cycle.intervals[i] = *(intervals + i);
  
  setCycle(type, steps, colors, loop);  
}

void CClyde::setCycle(ECycleType type, uint8_t steps, const RGB *colors, ECycleLoop loop) {
  m_cycle.type = type;
  m_cycle.numSteps = steps;
  
  for(int i = 0; i < steps; i++)
    m_cycle.colors[i] = *(colors+i);
  
  m_cycle.step = 0;
  m_cycle.stepStart = millis();
  m_cycle.stepEnd = m_cycle.stepStart + m_cycle.intervals[m_cycle.step];
  m_cycle.stepColor = RGB(m_ambient.color.r, m_ambient.color.g, m_ambient.color.b);
  m_cycle.loop = loop;
} 

void CClyde::stopCycle() {  //TODO should this be a function pointer set when starting cycle
  switch(m_cycle.type) {
    case SUNSET:
      //if current cycle is the afraid of the dark sunset,
      //then speed up and complete cycle
      //TODO setCycle could also set a pointer to a stop function...
      speedUpCycle(100);
      break;
    default:
      //for any other cycle, turn it off
      setAmbient(RGB(0,0,0));  //XXX this should move outside
      m_cycle.off();  
      break;
  }  
}

void CClyde::blink(const RGB& rgb, uint8_t numBlinks, uint8_t msInterval) {
  //calculate number of steps needed in the cycle
  uint8_t steps = numBlinks*2 + 1;
  
  //check number of step limit
  if (steps > CAmbientCycle::MAX_CYCLE_LENGTH)
    return;
  
  //set blinks color
  RGB colors[steps];
  uint8_t intervals[steps];
  
  for(int i = 0; i < steps; i++) {
    if (i%2==1)
      colors[i] = rgb;  
    intervals[i] = msInterval;
  }
  
  setCycle(BLINK, steps, &colors[0], &intervals[0], NO_LOOP);
}

void CClyde::updateCycle() {
  uint32_t now = millis();
  
  //if we reach the end of the current step
  if (now > m_cycle.stepEnd) {
    //process to next step
    updateCycleNextStep(now);
    //if we reached the end, then we're done
    if (!m_cycle.isOn())
      return;
  }

  //find where we are in the step 0-1
  float t;
  t = m_cycle.stepEnd - m_cycle.stepStart;
  t = t == 0 ? 1 : (now - m_cycle.stepStart) / t;

  //calculate the color for t
  RGB newColor;
  int16_t diff;
  diff = m_cycle.colors[m_cycle.step].r - m_cycle.stepColor.r;
  newColor.r = m_cycle.stepColor.r + (t*diff);
  diff = m_cycle.colors[m_cycle.step].g - m_cycle.stepColor.g;
  newColor.g = m_cycle.stepColor.g + (t*diff);
  diff = m_cycle.colors[m_cycle.step].b - m_cycle.stepColor.b;
  newColor.b = m_cycle.stepColor.b + (t*diff);

  setAmbient(newColor);
}

void CClyde::updateCycleNextStep(uint32_t now) {
  m_cycle.step++;

  //stop cycle if we reached the end
  if (m_cycle.step >= m_cycle.numSteps) {
    if (m_cycle.loop == LOOP) {
      m_cycle.step = 0;
    }
    else {
      m_cycle.type = OFF;
      setAmbient(m_cycle.colors[m_cycle.numSteps-1]);
      stop();
      return;
    }
  }

  m_cycle.stepStart = now;
  m_cycle.stepEnd += m_cycle.intervals[m_cycle.step];
  m_cycle.stepColor = RGB(m_ambient.color.r, m_ambient.color.g, m_ambient.color.b);
}

void CClyde::setCycleStep(uint8_t step) {
  if (step >= m_cycle.numSteps) return;
  
  m_cycle.step = step;
  m_cycle.stepStart = millis();
  m_cycle.stepEnd += m_cycle.intervals[m_cycle.step];
}

void CClyde::cycleNextStep(uint32_t now) {
  m_cycle.stepEnd = now;
  updateCycleNextStep(now);
}

void CClyde::speedUpCycle(uint32_t factor) {
  //reduce interval time to speed up end of cycle
  for(int i = 0; i < m_cycle.numSteps; i++) {
    m_cycle.intervals[i] /= factor;
    factor *= 2;
  }
  
  //jump cycle to next color
  cycleNextStep(millis());
}