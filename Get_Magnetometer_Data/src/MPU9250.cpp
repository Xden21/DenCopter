#include "MPU9250.h"

//==============================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer,
//====== and temperature data
//==============================================================================

void MPU9250::readGyroData()
{
	readGyroRaw();

	gx = ((float)gyroRaw[0] * gRes - gyroBias[0])*DEG_TO_RAD;
	gy = ((float)gyroRaw[1] * gRes - gyroBias[1])*DEG_TO_RAD;
	gz = ((float)gyroRaw[2] * gRes - gyroBias[2])*DEG_TO_RAD;
}

void MPU9250::readAccelData()
{
	readAccelRaw();

	ax = (float)accelRaw[0] * aRes - accelBias[0];
	ay = (float)accelRaw[1] * aRes - accelBias[1];
	az = (float)accelRaw[2] * aRes;
}

void MPU9250::readMagData()
{
	readMagRaw();

	//Invert mx and my values to match coördinate system of acc/gyro.
	float temp_mx = ((float)magRaw[0] * mRes) - magbias[0];
	float temp_my = ((float)magRaw[1] * mRes) - magbias[1];
	float temp_mz = ((float)magRaw[2] * mRes) - magbias[2];

	my = magscalebias[0][0] * temp_mx + magscalebias[0][1] * temp_my + magscalebias[0][2] * temp_mz;
	mx = magscalebias[1][0] * temp_mx + magscalebias[1][1] * temp_my + magscalebias[1][2] * temp_mz;
	mz = magscalebias[2][0] * temp_mx + magscalebias[2][1] * temp_my + magscalebias[2][2] * temp_mz;
}
void MPU9250::readSensor()
{
	if (dataReady())
	{
		readGyroData();
		readAccelData();
		readMagData();
	}
}

void MPU9250::getMres() {
	switch (Mscale)
	{
		// Possible magnetometer scales (and their register bit settings) are:
		// 14 bit resolution (0) and 16 bit resolution (1)
	case MFS_14BITS:
		mRes = 10.*4912. / 8190.; // Proper scale to return milliGauss
		break;
	case MFS_16BITS:
		mRes = 10.*4912. / 32760.0; // Proper scale to return milliGauss
		break;
	}
}

void MPU9250::getGres() {
	switch (Gscale)
	{
		// Possible gyro scales (and their register bit settings) are:
		// 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
			  // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
	case GFS_250DPS:
		gRes = 250.0 / 32768.0;
		break;
	case GFS_500DPS:
		gRes = 500.0 / 32768.0;
		break;
	case GFS_1000DPS:
		gRes = 1000.0 / 32768.0;
		break;
	case GFS_2000DPS:
		gRes = 2000.0 / 32768.0;
		break;
	}
}

void MPU9250::getAres() {
	switch (Ascale)
	{
		// Possible accelerometer scales (and their register bit settings) are:
		// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
			  // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
	case AFS_2G:
		aRes = 2.0 / 32768.0;
		break;
	case AFS_4G:
		aRes = 4.0 / 32768.0;
		break;
	case AFS_8G:
		aRes = 8.0 / 32768.0;
		break;
	case AFS_16G:
		aRes = 16.0 / 32768.0;
		break;
	}
}


void MPU9250::readAccelRaw()
{
	uint8_t rawData[6];  // x/y/z accel register data stored here
	readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
	accelRaw[0] = ((int16_t)rawData[0] << 8) | rawData[1];  // Turn the MSB and LSB into a signed 16-bit value
	accelRaw[1] = ((int16_t)rawData[2] << 8) | rawData[3];
	accelRaw[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}


void MPU9250::readGyroRaw()
{
	uint8_t rawData[6];  // x/y/z gyro register data stored here
	readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
	gyroRaw[0] = ((int16_t)rawData[0] << 8) | rawData[1];  // Turn the MSB and LSB into a signed 16-bit value
	gyroRaw[1] = ((int16_t)rawData[2] << 8) | rawData[3];
	gyroRaw[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}

void MPU9250::readMagRaw()
{
	// x/y/z gyro register data, ST2 register stored here, must read ST2 at end of
	// data acquisition
	uint8_t rawData[7];
	// Wait for magnetometer data ready bit to be set
	if (readByte(AK8963_ADDRESS, AK8963_ST1) & 0x01)
	{
		// Read the six raw data and ST2 registers sequentially into data array
		readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &rawData[0]);
		uint8_t c = rawData[6]; // End data read by reading ST2 register
		// Check if magnetic sensor overflow set, if not then report data
		if (!(c & 0x08))
		{
			// Turn the MSB and LSB into a signed 16-bit value
			magRaw[0] = ((int16_t)rawData[1] << 8) | rawData[0];
			// Data stored as little Endian 
			magRaw[1] = ((int16_t)rawData[3] << 8) | rawData[2];
			magRaw[2] = ((int16_t)rawData[5] << 8) | rawData[4];
		}
	}
}

int16_t MPU9250::readTempData()
{
	uint8_t rawData[2];  // x/y/z gyro register data stored here
	readBytes(MPU9250_ADDRESS, TEMP_OUT_H, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array 
	return ((int16_t)rawData[0] << 8) | rawData[1];  // Turn the MSB and LSB into a 16-bit value
}


void MPU9250::initAK8963(float * destination)
{
	// First extract the factory calibration for each magnetometer axis
	uint8_t rawData[3];  // x/y/z gyro calibration data stored here
	writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
	delay(10);
	writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
	delay(10);
	readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
	destination[0] = (float)(rawData[0] - 128) / 256. + 1.;   // Return x-axis sensitivity adjustment values, etc.
	destination[1] = (float)(rawData[1] - 128) / 256. + 1.;
	destination[2] = (float)(rawData[2] - 128) / 256. + 1.;
	writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
	delay(10);
	// Configure the magnetometer for continuous read and highest resolution
	// set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
	// and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
	writeByte(AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
	delay(10);
	std::cout << "AK8963 init done" << std::endl;
}

void MPU9250::initMPU9250()
{
	// wake up device
	writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
	delay(100); // Wait for all registers to reset 

	// get stable time source
	writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Auto select clock source to be PLL gyroscope reference if ready else
	delay(200);

	// Configure Gyro and Thermometer
	// Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively; 
	// minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
	// be higher than 1 / 0.0059 = 170 Hz
	// DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
	// With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
	writeByte(MPU9250_ADDRESS, CONFIG, 0x03);

	// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; a rate consistent with the filter update rate 
												   // determined inset in CONFIG above
	
	// Set gyroscope full scale range
	// Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
	uint8_t c = readByte(MPU9250_ADDRESS, GYRO_CONFIG); // get current GYRO_CONFIG register value
	// c = c & ~0xE0; // Clear self-test bits [7:5] 
	c = c & ~0x02; // Clear Fchoice bits [1:0] 
	c = c & ~0x18; // Clear AFS bits [4:3]
	c = c | Gscale << 3; // Set full scale range for the gyro
	// c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
	writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c); // Write new GYRO_CONFIG value to register

	// Set accelerometer full-scale range configuration
	c = readByte(MPU9250_ADDRESS, ACCEL_CONFIG); // get current ACCEL_CONFIG register value
	// c = c & ~0xE0; // Clear self-test bits [7:5] 
	c = c & ~0x18;  // Clear AFS bits [4:3]
	c = c | Ascale << 3; // Set full scale range for the accelerometer 
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

	// Set accelerometer sample rate configuration
	// It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
	// accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
	c = readByte(MPU9250_ADDRESS, ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
	c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
	c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value
	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
	// but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

	// Configure Interrupts and Bypass Enable
	// Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
	// clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips 
	// can join the I2C bus and all can be controlled by the Arduino as master
	writeByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x22);
	writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
	delay(100);
}


// Function which accumulates gyro and accelerometer data after device
// initialization. It calculates the average of the at-rest readings and then
// loads the resulting offsets into accelerometer and gyro bias registers.
void MPU9250::calibrateMPU9250(float * gyroBias, float * accelBias)
{
	us period{ (int)floor((1.0f / 200.0f * 1000000)) };
	//std::cout << period.count() << std::endl;
	us then = getTimeU();
	for (int i = 0; i < 128; i++)
	{
		readGyroData();
		readAccelData();
		gyroBias[0] += gx;
		gyroBias[1] += gy;
		gyroBias[2] += gz;
		accelBias[0] += ax;
		accelBias[1] += ay;
		accelBias[2] += az;
		us now = getTimeU();
		us proccessTime = now - then;
		//std::cout << proccessTime.count() << std::endl;
		if (proccessTime < period)
		{
			std::this_thread::sleep_for(period - proccessTime);
			//std::cout << "sleeping" << std::endl;
		}
		then = getTimeU();
	}
	gyroBias[0] /= 128.0f;
	gyroBias[1] /= 128.0f;
	gyroBias[2] /= 128.0f;
	accelBias[0] /= 128.0f;
	accelBias[1] /= 128.0f;
	accelBias[2] /= 128.0f;
	accelBias[2] -= 1; //account for gravity
	std::cout << gyroBias[0] << "    " << gyroBias[1] << "    " << gyroBias[2] << std::endl;
}

void MPU9250::calibrateMag()
{
	if (manualCalib)
	{
		std::fstream outputFile;
		outputFile.open("CalibrationData.txt", std::fstream::out | std::fstream::trunc);
		uint16_t ii = 0, sample_count = 0;
		int32_t mag_bias[3] = { 0, 0, 0 }, mag_scale[3] = { 0, 0, 0 };
		int16_t mag_max[3] = { -32767, -32767, -32767 }, mag_min[3] = { 32767, 32767, 32767 }, mag_temp[3] = { 0, 0, 0 };

		std::cout << "Mag Calibration: Wave device in a figure eight until done!" << std::endl;;
		sleep(4);

		// shoot for ~fifteen seconds of mag data
		if (Mmode == 0x02) sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms
		if (Mmode == 0x06) sample_count = 1500;  // at 100 Hz ODR, new mag data is available every 10 ms
		for (ii = 0; ii < sample_count; ii++) {
			readMagRaw();  // Read the mag data   
			for (int jj = 0; jj < 3; jj++) {
				if (magRaw[jj] > mag_max[jj]) mag_max[jj] = magRaw[jj];
				if (magRaw[jj] < mag_min[jj]) mag_min[jj] = magRaw[jj];
			}
			if (Mmode == 0x02) delay(135);  // at 8 Hz ODR, new mag data is available every 125 ms
			if (Mmode == 0x06) delay(12);  // at 100 Hz ODR, new mag data is available every 10 ms
		}


		// Get hard iron correction
		mag_bias[0] = (mag_max[0] + mag_min[0]) / 2;  // get average x mag bias in counts
		mag_bias[1] = (mag_max[1] + mag_min[1]) / 2;  // get average y mag bias in counts
		mag_bias[2] = (mag_max[2] + mag_min[2]) / 2;  // get average z mag bias in counts

		magbias[0] = (int)(mag_bias[0] * mRes*magCalibration[0]);  // save mag biases in G for main program
		magbias[1] = (int)(mag_bias[1] * mRes*magCalibration[1]);
		magbias[2] = (int)(mag_bias[2] * mRes*magCalibration[2]);

		outputFile << magbias[0] << "\n" << magbias[1] << "\n" << magbias[2] << "\n";

		// Get soft iron correction estimate
		mag_scale[0] = (mag_max[0] - mag_min[0]) / 2;  // get average x axis max chord length in counts
		mag_scale[1] = (mag_max[1] - mag_min[1]) / 2;  // get average y axis max chord length in counts
		mag_scale[2] = (mag_max[2] - mag_min[2]) / 2;  // get average z axis max chord length in counts

		float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
		avg_rad /= 3.0;


		//outputFile << magscalebias[0] << "\n" << magscalebias[1] << "\n" << magscalebias[2] << "\n";

		std::cout << "Mag Calibration done!" << std::endl;
		outputFile.close();
	}
	else
	{
		std::fstream inputFile;
		inputFile.open("CalibrationData.txt", std::fstream::in);
		//inputFile >> magbias[0] >> magbias[1] >> magbias[2] >> magscalebias[0] >> magscalebias[1] >> magscalebias[2];
		inputFile.close();
	}
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU9250::MPU9250SelfTest(float * destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
	uint8_t rawData[6] = { 0, 0, 0, 0, 0, 0 };
	uint8_t selfTest[6];
	int16_t gAvg[3], aAvg[3], aSTAvg[3], gSTAvg[3];
	float factoryTrim[6];
	uint8_t FS = 0;

	writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);    // Set gyro sample rate to 1 kHz
	writeByte(MPU9250_ADDRESS, CONFIG, 0x02);        // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
	writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 1 << FS);  // Set full scale range for the gyro to 250 dps
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, 0x02); // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 1 << FS); // Set full scale range for the accelerometer to 2 g

	for (int ii = 0; ii < 200; ii++) {  // get average current values of gyro and acclerometer

		readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);        // Read the six raw data registers into data array
		aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);  // Turn the MSB and LSB into a signed 16-bit value
		aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
		aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);

		readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);       // Read the six raw data registers sequentially into data array
		gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);  // Turn the MSB and LSB into a signed 16-bit value
		gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
		gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);
	}

	for (int ii = 0; ii < 3; ii++) {  // Get average of 200 values and store as average current readings
		aAvg[ii] /= 200;
		gAvg[ii] /= 200;
	}

	// Configure the accelerometer for self-test
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0); // Enable self test on all three axes and set accelerometer range to +/- 2 g
	writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
	delay(25);  // Delay a while to let the device stabilize

	for (int ii = 0; ii < 200; ii++) {  // get average self-test values of gyro and acclerometer

		readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
		aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);  // Turn the MSB and LSB into a signed 16-bit value
		aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
		aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);

		readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
		gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);  // Turn the MSB and LSB into a signed 16-bit value
		gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
		gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);
	}

	for (int ii = 0; ii < 3; ii++) {  // Get average of 200 values and store as average self-test readings
		aSTAvg[ii] /= 200;
		gSTAvg[ii] /= 200;
	}

	// Configure the gyro and accelerometer for normal operation
	writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);
	writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);
	delay(25);  // Delay a while to let the device stabilize

	// Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
	selfTest[0] = readByte(MPU9250_ADDRESS, SELF_TEST_X_ACCEL); // X-axis accel self-test results
	selfTest[1] = readByte(MPU9250_ADDRESS, SELF_TEST_Y_ACCEL); // Y-axis accel self-test results
	selfTest[2] = readByte(MPU9250_ADDRESS, SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
	selfTest[3] = readByte(MPU9250_ADDRESS, SELF_TEST_X_GYRO);  // X-axis gyro self-test results
	selfTest[4] = readByte(MPU9250_ADDRESS, SELF_TEST_Y_GYRO);  // Y-axis gyro self-test results
	selfTest[5] = readByte(MPU9250_ADDRESS, SELF_TEST_Z_GYRO);  // Z-axis gyro self-test results

	// Retrieve factory self-test value from self-test code reads
	factoryTrim[0] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[0] - 1.0))); // FT[Xa] factory trim calculation
	factoryTrim[1] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[1] - 1.0))); // FT[Ya] factory trim calculation
	factoryTrim[2] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[2] - 1.0))); // FT[Za] factory trim calculation
	factoryTrim[3] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[3] - 1.0))); // FT[Xg] factory trim calculation
	factoryTrim[4] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[4] - 1.0))); // FT[Yg] factory trim calculation
	factoryTrim[5] = (float)(2620 / 1 << FS)*(pow(1.01, ((float)selfTest[5] - 1.0))); // FT[Zg] factory trim calculation

   // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
   // To get percent, must multiply by 100
	for (int i = 0; i < 3; i++) {
		destination[i] = 100.0*((float)(aSTAvg[i] - aAvg[i])) / factoryTrim[i];   // Report percent differences
		destination[i + 3] = 100.0*((float)(gSTAvg[i] - gAvg[i])) / factoryTrim[i + 3]; // Report percent differences
	}
}

bool MPU9250::dataReady()
{
	return (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01);
}

// Wire.h read and write protocols
void MPU9250::writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	int _fd = wiringPiI2CSetup(address);
	if (_fd < 0) {
		fprintf(stderr, "Error: I2CSetup failed\n");
		//exit(EXIT_FAILURE);
	}
	if (wiringPiI2CWriteReg8(_fd, subAddress, data) < 0)
	{
		fprintf(stderr, "Error: Bad write");
	}
	close(_fd);
}

uint8_t MPU9250::readByte(uint8_t address, uint8_t subAddress)
{
	int _fd = wiringPiI2CSetup(address);
	if (_fd < 0) {
		fprintf(stderr, "Error: I2CSetup failed\n");
		//exit(EXIT_FAILURE);
	}
	uint8_t data = wiringPiI2CReadReg8(_fd, subAddress);
	close(_fd);
	return data;
}

void MPU9250::readBytes(uint8_t address, uint8_t subAddress, uint8_t count,
	uint8_t * dest)
{
	int _fd = wiringPiI2CSetup(address);
	if (_fd < 0) {
		fprintf(stderr, "Error: I2C Setup\n");
		exit(EXIT_FAILURE);
	}
	for (uint8_t i = 0; i < count; i++)
	{
		dest[i] = wiringPiI2CReadReg8(_fd, subAddress + i);
	}
	close(_fd);
	return;
}
