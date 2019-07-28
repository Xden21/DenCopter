#pragma once
/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks 
  ----> http://www.adafruit.com/products/
  ----> http://www.adafruit.com/products/

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#ifndef Adafruit_LEDBackpack_h
#define Adafruit_LEDBackpack_h

#include "../Comm/I2Ccom.h"
#include <map>

#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3
 


#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5


// this is the raw HT16K33 controller
class Adafruit_LEDBackpack {
 public:
  Adafruit_LEDBackpack(void);
  void begin(uint8_t _addr);
  void setBrightness(uint8_t b);
  void blinkRate(uint8_t b);
  void writeDisplay(void);
  void clear(void);

  uint8_t displaybuffer[16] = {};

  //void init(uint8_t a);
 protected:
  uint8_t i2c_addr;
};

class Display : public Adafruit_LEDBackpack {
 public:
  Display(void);

  void setDigitRaw(int pos, uint16_t bitmask);
  void setDecimal(int pos, bool decimal);
  void setDigit(int pos, std::string digit, bool decimal=false);
  void printString(std::string value, bool justify_right = false);
  void printNumberString(std::string value, bool justify_right = false);
  void writeStringToDisp(std::string value, bool dec=false, bool blink = false);

 private:


};
#endif // Adafruit_LEDBackpack_h

