/*
 Note: The MPU9250 is an I2C sensor and uses the Arduino Wire library.
 Because the sensor is not 5V tolerant, we are using a 3.3 V 8 MHz Pro Mini or
 a 3.3 V Teensy 3.1. We have disabled the internal pull-ups used by the Wire
 library in the Wire.h/twi.c utility file. We are also using the 400 kHz fast
 I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#define DEG_TO_RAD 0.01745329251994329576

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <fstream>
#include "MPU9250_Registers.h"
#include "../Comm/I2Ccom.h"

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

	enum LPF_BW {
		BW_100_HZ = 0x02, // 100HZ 3dB BW LPF
		BW_40_HZ = 0x03 // 40HZ 3dB BW LPF
	};

	// Specify sensor LPF BW
	uint8_t Lpf_BW = BW_40_HZ;
    // Specify sensor full scale
    uint8_t Gscale = GFS_500DPS;
    uint8_t Ascale = AFS_8G;
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
	float magCalibration[3] = { 0, 0, 0 };
	float magscalebias[3]; // magscalebias[3][3] = { { 1.015,0.005,-0.013 },{ 0.005,1.010,-0.010 },{ -0.013,0.010,0.976 } };
	float magbias[3] = { 36,97.4,-161.6 };
	bool newMagData = false;
    // Bias corrections for gyro and accelerometer
    float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};
    float SelfTest[6];
	bool manualCalib;


	void getMres();
	void getGres();
	void getAres();


	void readMPU9250Raw();
	void readAccelRaw();
	void readGyroRaw();
	void readMagRaw();

	int16_t readTempData();
	bool dataReady();

	int fileDescriptorIMU;
	int fileDescriptorMAG;

  public:
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
		fileDescriptorIMU = wiringPiI2CSetup(MPU9250_ADDRESS);
	}

	float ax, ay, az, gx, gy, gz, mx, my, mz;
};  // class MPU9250

#endif // _MPU9250_H_
