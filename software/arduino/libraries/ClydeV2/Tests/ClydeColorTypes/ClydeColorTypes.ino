#include <Wire.h>
#include <EEPROM.h>
#include <FabuleClydeDev.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>


#define FIRMWARE_VERSION 2

RGB color;
HSV hsv;
RGB rgb;
ColorConverter converter;

void setup() {
  Wire.begin();
  Clyde.begin();

  delay(2000);

  Serial.println("Clyde is Ready!");
  // color = RGB(255,0,255);
  // hsi = converter.RGBtoHSI(color);
  // rgb = converter.HSItoRGB(hsi);


}

void loop() {

  Serial.println("----------------------------------------");
  delay(2000);

  Serial.println("Clyde Color Conversion!");
  Serial.println();
  Serial.println();
  //white
  Serial.println("white");
  Serial.println();
  color = RGB(255,255,255);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //black
  Serial.println("black");
  Serial.println();
  color = RGB(0,0,0);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //red
  Serial.println("red");
  Serial.println();
  color = RGB(255,0,0);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //green
  Serial.println("green");
  Serial.println();
  color = RGB(0,255,0);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //blue
  Serial.println("blue");
  Serial.println();
  color = RGB(0,0,255);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //yellow
  Serial.println("yellow");
  Serial.println();
  color = RGB(255,255,0);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //magenta
  Serial.println("magenta");
  Serial.println();
  color = RGB(255,0,255);
  testColorCoversion(color);

  Serial.println();
  Serial.println();
  //cyan
  Serial.println("cyan");
  Serial.println();
  color = RGB(0,255,255);
  testColorCoversion(color);

  delay(20000);

}

void testColorCoversion(RGB color){
  //Set as RGB
  Clyde.ambientLight.setColor(color);
  delay(1000);
  //Set as HSV
  hsv = converter.RGBtoHSV(color);
  Clyde.ambientLight.setColor(hsv);
  delay(1000);
  //Set as RGB
  rgb = converter.HSVtoRGB(hsv);
  Clyde.ambientLight.setColor(rgb);
  delay(1000);
  //Off
  Clyde.ambientLight.setColor(RGB(0,0,0));
  delay(1000);
}

