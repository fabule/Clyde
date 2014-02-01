Librairies
==========
The Clyde library uses modified code from https://github.com/jrowberg/i2cdevlib to its I2C communication.

You do not need to install separate libraries to compile Clyde's firmware. The library only uses the "Wire" and "EEPROM" libraries, which are both part of the Arduino core. This is why you will find the following includes at the top of the "ClydeFirmware" sketch.
```
#include <Wire.h>
#include <EEPROM.h>
```
