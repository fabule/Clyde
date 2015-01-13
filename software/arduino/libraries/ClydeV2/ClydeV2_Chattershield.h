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

/**
 * Driver for VS1003 - MP3 / WMA / MIDI Audio Codec Chip
 *
 * See http://www.vlsi.fi/en/products/vs1003.html
 */

#ifndef __CHATTERSHIELD_H
#define __CHATTERSHIELD_H

#include <stdint.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SPI.h"

#undef PROGMEM
#define PROGMEM __attribute__ ((section (".progmem.data"))) 
#undef PSTR 
#define PSTR(s) (__extension__({static char __c[] PROGMEM = (s); &__c[0];}))

/****************************************************************************/

// VS1003 Audio decoder IC declarations:
#define VS_CMD_READ   3 //0b00000011  0x03  // Read  data
#define VS_CMD_WRITE  2 //0b00000010  0x02  // Write data

// VS1003 register addresses:
#define VS_SCI_MODE  (0x0)
#define VS_STATUS    (0x1)
#define VS_BASS      (0x2)
#define VS_CLOCKF    (0x3)
#define VS_DECOD_TIM (0x4)
#define VS_AUDATA    (0x5)
#define VS_WRAM      (0x6)
#define VS_WRAMADD   (0x7)
#define VS_SCI_HDAT0 (0x8)
#define VS_SCI_HDAT1 (0x9)
#define VS_AIADDR    (0xA)      // Dont touch!
#define VS_VOL       (0xB)
#define VS_AICTRL0   (0xC)      // Current tack #, also used to force to switch to new track
#define VS_AICTRL1   (0xD)      // total number of tracks found in SD card
#define VS_AICTRL2   (0xE)      // same as above for VS1103b IC (not us)
#define VS_AICTRL3   (0xF)      // Play mode, see bitmap below


/*  VS_AICTRL0 NOTES:
 * Write 0x8000 + song number to SCI AICTRL0 to jump to another song.
 * The high bit will be cleared when the song change is detected.
 * The pause mode (CTRL3 PAUSE ON), file ready (CTRL3 FILE READY), and paused at end (CTRL3 AT END) bits are automatically cleared.
 * If the song number is too large, playing restarts from the first file.
 * If you write to SCI AICTRL0 before starting the code, you can directly write the song number of the first song to play.
 * You can disable this feature (CTRL3 NO NUMFILES) to speed up the start of playback.*/

/// VS_AICTRL3 bit fields:

#define VS_CTRL3_UPDATE_VOL       (1<<15) // ’1’ = update volume (for UART control)
#define VS_CTRL3_I2S_ENABLE       (1<<9)  // Enable I2S output, VS1053 only
#define VS_CTRL3_BY_NAME        (1<<8)  // ’1’ = locate file by name
#define VS_CTRL3_AT_END         (1<<6)  // if PLAY MODE=3, 1=paused at end of file
#define VS_CTRL3_NO_NUMFILES      (1<<5)  // 0=normal, 1=do not count the number of files
#define VS_CTRL3_PAUSE_ON         (1<<4)  // 0=normal, 1=pause ON
#define VS_CTRL3_FILE_READY       (1<<3)  // 1=file found
#define VS_CTRL3_PLAY_NORM        (0)   // Default, cycle trough all tracks and start over forever
#define VS_CTRL3_PLAY_LOOP        (1<<1)  // Loop same track forever
#define VS_CTRL3_PLAY_PAUSE_1RST    (2<<1)  // pause before playing track
#define VS_CTRL3_PLAY_PAUSE_AFTER   (3<<1)  // pause after playing track
#define VS_CTRL3_RANDOM_PLAY      (1)   // shuffle play


/*  VS_AICTRL3 NOTES:
 * AICTRL3 should be set to the desired play mode by the user before starting the code.
 * If it is changed during play, care must be taken.
 * If the lowest bit of SCI AICTRL3 is 1, a random song is selected each time a new song starts.
 * The shuffle play goes through all files in random order, then plays them in a different order.
 * It can play a file twice in a row when a new random order is initiated.
 * The play mode mask bits can be used to change the default play behavior.
 * In normal mode the files are played one after another.
 * In loop song mode the playing file is repeated until a new file is selected.
 * CTRL3 FILE READY will be set to indicate a file was found and playing has started, but it will not be automatically cleared.
 * Pause before play mode will first locate the file, then go to pause mode.
 * CTRL3 PAUSE ON will get set to indicate pause mode, CTRL3 FILE READY will be set to indicate a file was found.
 * When the user has read the file ready indicator, he should reset the file ready bit.
 * The user must also reset the CTRL3 PAUSE ON bit to start playing.
 * One use for the pause before play mode is scanning the file names.
 * Pause after play mode will play files normally, but will go to pause mode and set the CTRL3 AT END bit right after finishing a file.
 * AICTRL0 will be increased to point to the next file (or the number of files if the song played was the last file),
 * but this file is not yet ready to play. CTRL3 PAUSE ON will get set to indicate pause mode;
 * The user must reset the CTRL3 PAUSE ON bit to move on to locate the next file,
 * or select a new file by writing 0x8000 + song number to AICTRL0.
 * CTRL3 PAUSE ON, CTRL3 FILE READY, and CTRL3 AT END bits are automatically cleared when new file is selected through AICTRL0.
 * Pause after play and loop mode are only checked when the file has been fully read.
 * Pause before play is checked after the file has been located, but before the actual playing starts.
 * Take this into account if you want to change playing mode while files are playing. */


/* When a file has been selected, the MSDOS short filename (8+3 characters) can be read from VS10xx memory.
 * The filename is in Y memory at addresses 0x780..0x785 (VS1011B,VS1011E, VS1002D) or 0x1800..0x1805 (VS1003B, VS1033C).
 * The first character is in the most-significant bits of the first word.  */

// VS1003 SCI_MODE register bit fields:

//#define VS_NewSPImode_bM  (0x0800)
                  // MASK
#define VS_SM_DIFF     (1)   // 1    1 = left channel inverted
// bit one is "SM_SETTOZERO"    // 0x0002
#define VS_SM_RESET    (1<<2)  // 0x0004 1 = Soft reset
#define VS_SM_OUTOFWAV (1<<3)  // 0x0008 1 = Jump out of WAV decoding
#define VS_SM_PDOWN    (1<<4)  // 0x0010 1 = powerdown
#define VS_SM_TESTS    (1<<5)  // 0x0020 1 = Allow SDI tests
#define VS_SM_STREAM   (1<<6)  // 0x0040 1 = Stream mode
#define VS_SM_ERSPKR (1<<7)  // 0x0040 1 = Stream mode
// bit one is "SM_SETTOZERO2"   // 0x0080
#define VS_SM_DACT     (1<<8)  // 0x0100 1 = falling DCLK active edge
#define VS_SM_SDIORD   (1<<9)  // 0x0200 1 = MSb last SDI bit order
#define VS_SM_SDISHARE  (1<<10) // 0x0400 1 = Share SPI chip select
#define VS_SM_SDINEW   (1<<11) // 0x0800 1 = VS1002 native SPI modes (default)
#define VS_SM_ADPCM    (1<<12) // 0x1000 1 = ADPCM recording active
#define VS_SM_ADPCM_HP   (1<<13) // 0x2000 1 = ADPCM high-pass filter active
#define VS_SM_LINE_IN  (1<<14) // 0x4000 1 = line in / 0 = MIC: ADPCM recording selector
//#define VS__bM  (1<<) //


/**
 * CChattershield class handles communication to the VS1003 
 */
class ClydeV2_Chattershield {

public:
  /**
   * Constructor
   *
   * Only sets pin values.  Doesn't do touch the chip.  Be sure to call begin()!
   */
  ClydeV2_Chattershield(uint8_t _cs_pin, uint8_t _dcs_pin, uint8_t _dreq_pin);

  /**
   * Begin operation
   *
   * Sets pins correctly, and prepares SPI bus.
   */
  bool init(void);

  void pause(void) const;

  void fadeOut(void) const;

  void play(void) const;

  void playLoop(void) const;

  void playTrack(uint16_t trackNumber, bool loop);

  uint16_t getTrackCount(void) const;

  uint16_t getCurrentTrackNumber(void) const;

  /**
   * Set the player volume
   *
   * @param vol Volume level from 0-255, lower is louder.
   */
  void setVolume(uint8_t vol) const;

  /**
   * Load VS1003 firmware from EEPROM
   */
  void loadEEPROM(void) const;

  /** FROM ARDUINO VS1003 LIB  */
private:

  uint8_t cs_pin; /**< Pin where CS line is connected */
  uint8_t dcs_pin; /**< Pin where DCS line is connected */
  uint8_t dreq_pin; /**< Pin where DREQ line is connected */
  
  #ifdef SPI_HAS_TRANSACTION
    SPISettings spiSettings;
  #endif

  protected:
  void boot(void) const;
  bool detect(void) const;

  inline void await_data_request(void) const
  {
    while ( !digitalRead(dreq_pin) );
  }

  inline void control_mode_on(void) const
  {
    digitalWrite(dcs_pin,HIGH);
    digitalWrite(cs_pin,LOW);
  }

  inline void control_mode_off(void) const
  {
    digitalWrite(cs_pin,HIGH);
  }

  inline void data_mode_on(void) const
  {
    digitalWrite(cs_pin,HIGH);
    digitalWrite(dcs_pin,LOW);
  }

  inline void data_mode_off(void) const
  {
    digitalWrite(dcs_pin,HIGH);
  }
  
  uint16_t read_register(uint8_t _reg) const;
  void write_register(uint8_t _reg,uint16_t _value) const;
  void softReset() const;

};

#endif