// Clyde Configuration

#ifndef __CLYDE_CONFIG_H
#define __CLYDE_CONFIG_H

// Pins

static const uint8_t EYE_PIN = 0;//The SquishyEye analog pin

static const uint8_t R_PIN = 5;//The RED pin
static const uint8_t G_PIN = 6;//The GREEN pin
static const uint8_t B_PIN = 9;//The BLUE pin
static const uint8_t TASK_PIN = 11;//The TASK LIGHT pin

static const uint8_t MODULE_1_DPIN = 7;//The Module 1 digital pin
static const uint8_t MODULE_1_APIN = 1;//The Module 1 analog pin
static const uint8_t MODULE_2_DPIN = 8;//The Module 2 digital pin
static const uint8_t MODULE_2_APIN = 2;//The Module 2 analog pin

static const uint8_t CHATTER_CS_PIN = 4;//Chatter cs pin
static const uint8_t CHATTER_DCS_PIN = 12;//Chatter dcs pin
static const uint8_t CHATTER_DREQ_PIN = 13;//Chatter dreq pin
static const uint8_t CHATTER_NUM_TRACKS = 13;//The minimum number of tracks on the Chatter microSD card


// Modules

#define ENABLE_AFRAID_OF_THE_DARK
#define ENABLE_TOUCHY_FEELY
#define ENABLE_CHATTERBOX

#endif

