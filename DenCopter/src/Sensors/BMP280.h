#ifndef __BMP280_H__
#define	__BMP280_H__

#include "../Comm/I2Ccom.h"
#include <iostream>
#include <cmath>

/*=========================================================================
I2C ADDRESS/BITS/SETTINGS
-----------------------------------------------------------------------*/
#define BMP280_ADDRESS                (0x77)
#define BMP280_CHIPID                 (0x58)
/*=========================================================================*/

/*=========================================================================
REGISTERS
-----------------------------------------------------------------------*/
enum
{
	BMP280_REGISTER_DIG_T1 = 0x88,
	BMP280_REGISTER_DIG_T2 = 0x8A,
	BMP280_REGISTER_DIG_T3 = 0x8C,

	BMP280_REGISTER_DIG_P1 = 0x8E,
	BMP280_REGISTER_DIG_P2 = 0x90,
	BMP280_REGISTER_DIG_P3 = 0x92,
	BMP280_REGISTER_DIG_P4 = 0x94,
	BMP280_REGISTER_DIG_P5 = 0x96,
	BMP280_REGISTER_DIG_P6 = 0x98,
	BMP280_REGISTER_DIG_P7 = 0x9A,
	BMP280_REGISTER_DIG_P8 = 0x9C,
	BMP280_REGISTER_DIG_P9 = 0x9E,

	BMP280_REGISTER_CHIPID = 0xD0,
	BMP280_REGISTER_VERSION = 0xD1,
	BMP280_REGISTER_SOFTRESET = 0xE0,

	BMP280_REGISTER_CAL26 = 0xE1,  // R calibration stored in 0xE1-0xF0

	BMP280_REGISTER_STATUS = 0xF3,
	BMP280_REGISTER_CONTROL = 0xF4,
	BMP280_REGISTER_CONFIG = 0xF5,
	BMP280_REGISTER_PRESSUREDATA = 0xF7,
	BMP280_REGISTER_TEMPDATA = 0xFA,
};

/*=========================================================================*/



class BMP280
{
public:
	BMP280();
	
	int begin();		//Initialize sensor
	void calibrate();	//Calibrate ground pressure
	float getAltitude();//Calculate current altitude
private:
	void readCoefficients();
	void readRaw();
	void readTemperature();
	void readPressure();
	bool dataReady();

	int fileDescriptorBMP280;


	//Measurements
	float pressure = 0;       //in hPa
	float temperature = 0;	  //in °C
	float altitude = 0;
	float calib_pressure = 0; //In hPa

	uint8_t rawData[6]{ 0 };
	int32_t t_fine = 0;
	//control setting 0xF4
	uint8_t osrs_t = 5;		//Temp resolution
	uint8_t osrs_p = 5;		//Pressure resolution
	uint8_t mode = 3;		//Power mode

	//config setting 0xF5
	uint8_t t_sb = 0;		//Standby time
	uint8_t filter = 4;		//Filter coefficient
	uint8_t spi3w_en = 0;	//SPI 3 wire enable (not needed)
	/*
		Gives ODR of: 26.32Hz so sample at 30Hz
	*/

	uint16_t read16_LE(uint8_t reg);
	int16_t readS16_LE(uint8_t reg);


	/*=========================================================================
	CALIBRATION DATA
	-----------------------------------------------------------------------*/
	typedef struct
	{
		uint16_t dig_T1;
		int16_t  dig_T2;
		int16_t  dig_T3;

		uint16_t dig_P1;
		int16_t  dig_P2;
		int16_t  dig_P3;
		int16_t  dig_P4;
		int16_t  dig_P5;
		int16_t  dig_P6;
		int16_t  dig_P7;
		int16_t  dig_P8;
		int16_t  dig_P9;

		uint8_t  dig_H1;
		int16_t  dig_H2;
		uint8_t  dig_H3;
		int16_t  dig_H4;
		int16_t  dig_H5;
		int8_t   dig_H6;
	} bmp280_calib_data;

	bmp280_calib_data _bmp280_calib;
	/*=========================================================================*/
};

#endif // !__BMP280_H__
