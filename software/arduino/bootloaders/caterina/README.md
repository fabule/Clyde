Clyde Bootloader
================

How-To Compile
--------------
* Download and extract LUFA: https://code.google.com/p/lufa-lib/downloads/detail?name=LUFA-111009.zip&can=2&q=
* Set your LUFA directory in the Makefile.
* Run: make clyde

How-To Program
--------------
* Set your AVRDUDE_PROGRAMMER and AVRDUDE_PORT in the Makefile
* Run: make program
* (on Windows) You'll need to point to the .inf file found in /software/arduino/drivers

Changes
-------
* Added "Fabule" and "Clyde" to the USB descriptor strings of Descriptor.c
* Added a "clyde" target to the Makefile to speed up the compiling process
* Changed the "program" target of the Makefile to accept product ids for hex file selection

Attribution
-----------
This Clyde bootloader is based on the Arduino Caterina bootloader in release 1.0.5 found at:
https://github.com/arduino/Arduino/releases/tag/1.0.5

This bootloader, just like the original Caterina, uses the LUFA library found at:
https://code.google.com/p/lufa-lib/
more specifically, it was tested with the "111009" release of LUFA found at:
https://code.google.com/p/lufa-lib/downloads/detail?name=LUFA-111009.zip&can=2&q=
