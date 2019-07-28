/*
 Note: The MPU9250 is an I2C sensor and uses the Arduino Wire library.
 Because the sensor is not 5V tolerant, we are using a 3.3 V 8 MHz Pro Mini or
 a 3.3 V Teensy 3.1. We have disabled the internal pull-ups used by the Wire
 library in the Wire.h/twi.c utility file. We are also using the 400 kHz fast
 I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#define DEG_TO_RAD M_PI/180.0f

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <thread>
#include <fstream>
#include "MPU9250_Registers.h"
#include "Timer.h"

class MPU9250
{
  protected:
    // Set initial input parameters
    enum Ascale {
      AFS_2G = 0,
      AFS_4G,
      AFS_8G,
      AFS_16G
    };

    enum Gscale {
      GFS_250DPS = 0,
      GFS_500DPS,
      GFS_1000DPS,
      GFS_2000DPS
    };

    enum Mscale {
      MFS_14BITS = 0, // 0.6 mG per LSB
      MFS_16BITS      // 0.15 mG per LSB
    };

    // Specify sensor full scale
    uint8_t Gscale = GFS_250DPS;
    uint8_t Ascale = AFS_4G;
    // Choose either 14-bit or 16-bit magnetometer resolution
    uint8_t Mscale = MFS_16BITS;
    // 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read
    uint8_t Mmode = 0x06;
    float pitch, yaw, roll;
    float temperature;   // Stores the real internal chip temperature in Celsius
    int16_t tempCount;   // Temperature raw count output

   

    // Scale resolutions per LSB for the sensors
    float aRes, gRes, mRes;
    // Variables to hold latest sensor data values
	int16_t gyroRaw[3];   // Stores the 16-bit signed gyro sensor output
	int16_t magRaw[3];    // Stores the 16-bit signed magnetometer sensor output
	int16_t accelRaw[3]; // Stores the 16-bit signed accelerometer sensor output
    // Factory mag calibration and mag bias
	float magCalibration[3] = { 0, 0, 0 }, magscalebias[3][3] = { { 1.015,0.005,-0.013 },{0.005,1.010,-0.010},{-0.013,0.010,0.976} };
	float magbias[3] = {36,97.4,-161.6};
    // Bias corrections for gyro and accelerometer
    float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};
    float SelfTest[6];
	bool manualCalib;

	void getMres();
	void getGres();
	void getAres();
	void readAccelRaw();
	void readGyroRaw();
	void readMagRaw();
	int16_t readTempData();
	bool dataReady();
	void writeByte(uint8_t, uint8_t, uint8_t);
	uint8_t readByte(uint8_t, uint8_t);
	void readBytes(uint8_t, uint8_t, uint8_t, uint8_t *);
public:

	//TEMP
	 float ax, ay, az, gx, gy, gz, mx, my, mz;
	 //

    void initAK8963(float *);
    void initMPU9250();
    void calibrateMPU9250(float * gyroBias, float * accelBias);
	void calibrateMag();
    void MPU9250SelfTest(float * destination);
	void readAccelData();
	void readGyroData();
	void readMagData();
	void readSensor();

	MPU9250(bool manual_calib)
		:manualCalib{ manual_calib }
	{

	}
    
};  // class MPU9250

#endif // _MPU9250_H_
