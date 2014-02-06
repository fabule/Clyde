Clyde
======
Clyde is a lamp with a lot of personality that you can adapt, play with, and truly call your own. Clyde is open source and Arduino-compatible.  You can change how Clyde reacts to his environment by inserting personality modules on his main controller board. You can also change how Clyde behaves by reprogramming his firmware using the standard Arduino IDE.

For more information, see the website at: http://www.fabule.com or the forum at: http://fabule.com/forum/

To report a bug in the hardware or software, go to:
http://github.com/fabule/Clyde/issues

Installation
------------

The Clyde firmware is divided into three parts: bootloader, library, and sketch.

You can find the Clyde library in /software/arduino/libraries/. Place the "clyde" library folder inside your Arduino "librairies" folder. This contains the majority of the code that controls Clyde. If you are not sure about how to install an Arduino library, you can find instructions here: http://arduino.cc/en/Guide/Libraries

When the library is in place, you can look for the sketch in /software/arduino/firmwares/. Place the "ClydeFirmware" sketch folder inside your Arduino "sketches" folder. This sketch does not contain much, but it is a good place to start playing with Clyde's behaviors.

Before you open your Arduino IDE, you'll need to copy one more folder. This step will add Clyde as a device under the "Tools > Board menu". First, if you do not have a "hardware" folder at the same level as your Arduino "librairies" folder, time to create one. Browse to the "hardware" folder in /software/arduino/hardware/ and open the subfolder that matches the version of your Arduino IDE, 1.0.5 or 1.5. Inside you'll find a "fabule" folder. Copy the "fabule" folder, inside the Arduino "hardware" folder you just created.

At this point, you should be ready to compile and upload Clyde's firmware. Connect Clyde to your computer using the USB cable. If you are using Windows, you'll need to point to Clyde's driver found in /software/arduino/drivers/. Open the "ClydeFirmware" sketch in your Arduino IDE. Select Clyde under the "Tools > Board" menu, and the port assigned to Clyde under "Tools > Serial" Port. Press "Upload", and the firmware should compile and upload into Clyde.

If you want to make modifications to the bootloader, please refer to the information found here: https://github.com/fabule/Clyde/tree/master/software/arduino/bootloaders/caterina

Credits
-------
Clyde is an open source project manage by Fabule Fabrications in Montréal, Québec, Canada.

Clyde is based on the Arduino project, see the website at: http://arduino.cc

Arduino uses the GNU avr-gcc toolchain, avrdude, avr-libc, and code from
Processing and Wiring, LUFA, and i2cdevlib.