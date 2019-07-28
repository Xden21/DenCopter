
#include "BMP280.h"

BMP280::BMP280()
{
	
	if (!begin())
	{
		std::cerr << "Failed to init BMP280" << std::endl;
		std::cin.ignore();
		std::cin.get();

		exit(EXIT_FAILURE);
	}
}

void BMP280::calibrate()
{
	while (!dataReady())
		;
	readPressure();
	calib_pressure = pressure;
}

float BMP280::getAltitude()
{
	while (!dataReady())
		;
	readPressure();
	altitude = ((pow((calib_pressure / pressure), 1 / 5.257) - 1.0) * (temperature + 273.15)) / 0.0065;
	return altitude;
}

int BMP280::begin()
{
	fileDescriptorBMP280 = wiringPiI2CSetup(BMP280_ADDRESS);

	if (readReg8(fileDescriptorBMP280, BMP280_REGISTER_CHIPID) != BMP280_CHIPID)
	{
		return 0;
	}
	readCoefficients();
	uint8_t temp = 0;
	temp = temp | mode;
	temp = temp | (osrs_p << 2);
	temp = temp | (osrs_t << 5);
	writeReg8(fileDescriptorBMP280, BMP280_REGISTER_CONTROL, temp);

	temp = 0;
	temp = temp | spi3w_en;
	temp = temp | (filter << 2);
	temp = temp | (t_sb << 5);
	writeReg8(fileDescriptorBMP280, BMP280_REGISTER_CONFIG, temp);

	usleep(0.5 * 1000000);
	while (!dataReady())
		;
	readPressure();
	calib_pressure = pressure;
	return 1;
}

void BMP280::readCoefficients()
{
	_bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
	_bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
	_bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

	_bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
	_bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
	_bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
	_bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
	_bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
	_bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
	_bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
	_bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
	_bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

void BMP280::readRaw()
{
	uint8_t tempData[6]{};
	for (int i = 0; i < 6; i++)
	{
		tempData[i] = rawData[i];
	}
	if (readRegList(fileDescriptorBMP280, BMP280_REGISTER_PRESSUREDATA, tempData, 6))
	{
		std::cout << "BMP280: Bad data read" << std::endl;
		return;
	}
	for (int i = 0; i < 6; i++)
	{
		rawData[i] = tempData[i];
	}
}

void BMP280::readTemperature()
{
	int32_t var1, var2;

	int32_t adc_T = (int32_t)((((int32_t)(rawData[3])) << 12) 
		| (((int32_t)(rawData[4])) << 4) | (((int32_t)(rawData[5])) >> 4));

	var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *
		((int32_t)_bmp280_calib.dig_T2)) >> 11;

	var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *
		((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
		((int32_t)_bmp280_calib.dig_T3)) >> 14;

	t_fine = var1 + var2;

	temperature = (float)((t_fine * 5 + 128) >> 8)/100;
}

void BMP280::readPressure()
{
	int64_t var1, var2, p;
	readTemperature();
	int32_t adc_P = (int32_t)((((uint32_t)(rawData[0])) << 12) 
		| (((uint32_t)(rawData[1])) << 4) | ((uint32_t)rawData[2] >> 4));
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
	var2 = var2 + ((var1*(int64_t)_bmp280_calib.dig_P5) << 17);
	var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
		((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1))*((int64_t)_bmp280_calib.dig_P1) >> 33;

	if (var1 == 0) {
		return;  // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

	pressure = (float)((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4)/256000;	//in hPa
}

bool BMP280::dataReady()
{
	return (((readReg8(fileDescriptorBMP280, BMP280_REGISTER_STATUS) & 0x08) >> 3) == 0);
}

uint16_t BMP280::read16_LE(uint8_t reg)
{
	uint16_t temp = readReg16(fileDescriptorBMP280, reg);
	return (temp >> 8) | (temp << 8);
}

int16_t BMP280::readS16_LE(uint8_t reg)
{
	return (int16_t)read16_LE(reg);
}
