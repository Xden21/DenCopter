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

#include "Display.h"

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
 
std::map <std::string, uint16_t> DIGIT_VALUES
{
	{ " ", 0b0000000000000000 },
	{ "!" , 0b0000000000000110 },
	{ "\"" , 0b0000001000100000 },
	{ "#" , 0b0001001011001110 },
	{ "$" , 0b0001001011101101 },
	{ "%" , 0b0000110000100100 },
	{ "&" , 0b0010001101011101 },
	{ "\"" , 0b0000010000000000 },
	{ "(" , 0b0010010000000000 },
	{ ")" , 0b0000100100000000 },
	{ "*" , 0b0011111111000000 },
	{ "+" , 0b0001001011000000 },
	{ "," , 0b0000100000000000 },
	{ "-" , 0b0000000011000000 },
	{ "." , 0b0000000000000000 },
	{ "/" , 0b0000110000000000 },
	{ "0" , 0b0000110000111111 },
	{ "1" , 0b0000000000000110 },
	{ "2" , 0b0000000011011011 },
	{ "3" , 0b0000000010001111 },
	{ "4" , 0b0000000011100110 },
	{ "5" , 0b0010000001101001 },
	{ "6" , 0b0000000011111101 },
	{ "7" , 0b0000000000000111 },
	{ "8" , 0b0000000011111111 },
	{ "9" , 0b0000000011101111 },
	{ ":" , 0b0001001000000000 },
	{ ";" , 0b0000101000000000 },
	{ "<" , 0b0010010000000000 },
	{ "=" , 0b0000000011001000 },
	{ ">" , 0b0000100100000000 },
	{ "?" , 0b0001000010000011 },
	{ "@" , 0b0000001010111011 },
	{ "A" , 0b0000000011110111 },
	{ "B" , 0b0001001010001111 },
	{ "C" , 0b0000000000111001 },
	{ "D" , 0b0001001000001111 },
	{ "E" , 0b0000000011111001 },
	{ "F" , 0b0000000001110001 },
	{ "G" , 0b0000000010111101 },
	{ "H" , 0b0000000011110110 },
	{ "I" , 0b0001001000000000 },
	{ "J" , 0b0000000000011110 },
	{ "K" , 0b0010010001110000 },
	{ "L" , 0b0000000000111000 },
	{ "M" , 0b0000010100110110 },
	{ "N" , 0b0010000100110110 },
	{ "O" , 0b0000000000111111 },
	{ "P" , 0b0000000011110011 },
	{ "Q" , 0b0010000000111111 },
	{ "R" , 0b0010000011110011 },
	{ "S" , 0b0000000011101101 },
	{ "T" , 0b0001001000000001 },
	{ "U" , 0b0000000000111110 },
	{ "V" , 0b0000110000110000 },
	{ "W" , 0b0010100000110110 },
	{ "X" , 0b0010110100000000 },
	{ "Y" , 0b0001010100000000 },
	{ "Z" , 0b0000110000001001 },
	{ "[" , 0b0000000000111001 },
	{ "\\" , 0b0010000100000000 },
	{ "]" , 0b0000000000001111 },
	{ "^" , 0b0000110000000011 },
	{ "_" , 0b0000000000001000 },
	{ "`" , 0b0000000100000000 },
	{ "a" , 0b0001000001011000 },
	{ "b" , 0b0010000001111000 },
	{ "c" , 0b0000000011011000 },
	{ "d" , 0b0000100010001110 },
	{ "e" , 0b0000100001011000 },
	{ "f" , 0b0000000001110001 },
	{ "g" , 0b0000010010001110 },
	{ "h" , 0b0001000001110000 },
	{ "i" , 0b0001000000000000 },
	{ "j" , 0b0000000000001110 },
	{ "k" , 0b0011011000000000 },
	{ "l" , 0b0000000000110000 },
	{ "m" , 0b0001000011010100 },
	{ "n" , 0b0001000001010000 },
	{ "o" , 0b0000000011011100 },
	{ "p" , 0b0000000101110000 },
	{ "q" , 0b0000010010000110 },
	{ "r" , 0b0000000001010000 },
	{ "s" , 0b0010000010001000 },
	{ "t" , 0b0000000001111000 },
	{ "u" , 0b0000000000011100 },
	{ "v" , 0b0010000000000100 },
	{ "w" , 0b0010100000010100 },
	{ "x" , 0b0010100011000000 },
	{ "y" , 0b0010000000001100 },
	{ "z" , 0b0000100001001000 },
	{ "{" , 0b0000100101001001 },
	{ "|" , 0b0001001000000000 },
	{ "}" , 0b0010010010001001 },
	{ "~" , 0b0000010100100000 }
};

void Adafruit_LEDBackpack::setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  write8(i2c_addr, HT16K33_CMD_BRIGHTNESS | b);
}

void Adafruit_LEDBackpack::blinkRate(uint8_t b) {
	if (b > 3) b = 0; // turn off if not sure
	write8(i2c_addr, HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1));
}

Adafruit_LEDBackpack::Adafruit_LEDBackpack(void) {
}

void Adafruit_LEDBackpack::begin(uint8_t _addr = 0x70) {
	i2c_addr = wiringPiI2CSetup(_addr);
	if (write8(i2c_addr, 0) < 0)
		std::cerr << "Could not communicate with disp!" << std::endl;

	write8(i2c_addr, 0x21);
	blinkRate(HT16K33_BLINK_OFF);
  
	setBrightness(15); // max brightness
}

void Adafruit_LEDBackpack::writeDisplay(void) {
	writeRegList(i2c_addr, 0, displaybuffer, 16);
}

void Adafruit_LEDBackpack::clear(void) {
  for (uint8_t i=0; i<8; i++) {
    displaybuffer[i] = 0;
  }
}

/******************************* QUAD ALPHANUM OBJECT */

Display::Display(void) {
	//std::cout << "Beginning display" << std::endl;
	begin();
	clear();
	writeStringToDisp("BOOT");
}

void Display::setDigitRaw(int pos, uint16_t bitmask)
{
	if (pos < 0 || pos>3)
		return;
	displaybuffer[2 * pos] = bitmask & 0xFF;
	displaybuffer[2 * pos + 1] = (bitmask >> 8) & 0xFF;
}

void Display::setDecimal(int pos, bool decimal)
{
	if (pos < 0 || pos>3)
		return;
	if (decimal)
		displaybuffer[pos * 2 + 1] |= (1 << 6);
	else
		displaybuffer[pos * 2 + 1] &= ~(1 << 6);
}

void Display::setDigit(int pos, std::string digit, bool decimal)
{
	setDigitRaw(pos, DIGIT_VALUES[digit]);
	if (decimal)
		setDecimal(pos, true);
}

void Display::printString(std::string value, bool justify_right)
{
	int pos = justify_right ? (4 - value.length()) : 0;
	for (int i = 0; i < value.length(); i++)
	{
		setDigit(pos + i, value.substr(i,1));
	}
}

void Display::printNumberString(std::string value, bool justify_right)
{
	size_t pointindex = value.find_first_of(".");
	std::string temp = value.substr(0, pointindex - 1).append(value.substr(pointindex + 1));
	if (temp.length() > 4)
		return;
	int pos = justify_right ? (4 - temp.length()) : 0;
	for (int i = 0; i < value.length(); i++)
	{
		if (value.at(i) == '.')
		{
			setDecimal(pos - 1, true);
		}
		else
		{
			setDigit(pos, value.substr(i, 1));
			pos += 1;
		}
	}
}

void Display::writeStringToDisp(std::string value, bool dec, bool blink)
{
	clear();
	if (dec)
		printNumberString(value);
	else
		printString(value);
	writeDisplay();
	if (blink)
		blinkRate(HT16K33_BLINK_HALFHZ);
	else
		blinkRate(HT16K33_BLINK_OFF);
}