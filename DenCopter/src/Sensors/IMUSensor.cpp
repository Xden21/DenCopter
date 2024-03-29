/*///////////////////////////////////////////////////
//  Madgwick filter class functions implementations//
//  Used for interpreting sensor data              //                                
///////////////////////////////////////////////////*/


//Includes


/////////////////////////////////////////////////////
//Public functions

#include "IMUSensor.h"

static void Sensor_ISR(void)
{
	newSensorMeasurement = true;
}

// Initialise the sensor (See MPU9250 functions for more info)
void IMUSensor::initSensor()
{
	if (!initDone)
	{
		//Reset all registers
		writeReg8(fileDescriptorIMU,PWR_MGMT_1, 0x80);
		delay(100);

		if (readReg8(fileDescriptorIMU, WHO_AM_I_MPU9250) != 0x71)
			std::cerr << "NO COM WITH MPU9250!" << std::endl;
		else
			std::cout << "Com with MPU9250 succesfull" << std::endl;

		// Self test, puts deviations in SelfTest array.
		MPU9250SelfTest(SelfTest);
		std::cout << "x-axis self test: acceleration trim within : " << SelfTest[0] << " % of factory value" << std::endl;
		std::cout << "y-axis self test: acceleration trim within : " << SelfTest[1] << " % of factory value" << std::endl;
		std::cout << "z-axis self test: acceleration trim within : " << SelfTest[2] << " % of factory value" << std::endl;
		std::cout << "x-axis self test: gyration trim within : " << SelfTest[3] << " % of factory value" << std::endl;
		std::cout << "y-axis self test: gyration trim within : " << SelfTest[4] << " % of factory value" << std::endl;
		std::cout << "z-axis self test: gyration trim within : " << SelfTest[5] << " % of factory value" << std::endl;
		delay(1000);

		// Get output resolutions.
		getAres();
		getMres();
		getGres();

		// Calibrate MPU9250
		calibrateMPU9250(gyroBias, accelBias);
		std::cout << "accel biases (mg): " << 1000. * accelBias[0] << "     " << 1000. * accelBias[1] << "     " << 1000. * accelBias[2] << std::endl;
		std::cout << "gyro biases(dps): " << gyroBias[0] << "     " << gyroBias[1] << "     " << gyroBias[2] << std::endl;
		delay(2000);

		// Init Sensors
		// Inits all regs and sets output rate, DLPS, and interrupt which triggers at 250hz.
		initMPU9250();
		delay(100);
		initAK8963(magCalibration);
		delay(100);
		//Calibrate Magnetometer, saves offsets and scale factors in correct arrays.		
		if (manualCalib) 
		{
			calibrateMag();
			delay(1000);
		}
		
		// Start interrupt handler.
		wiringPiISR(INT_PIN, INT_EDGE_RISING, &Sensor_ISR);

		initDone = true;
		_then = getTimeU();
	}
}


// Get the normalised gyroscope data.
void IMUSensor::getGyro()
{
	while (!newSensorMeasurement)
		;
	readGyroData();
}


// Calculate the angle with the Madgwick sensor fusion algorithm.
void IMUSensor::getAnglesMadgwick()
{
    if (!initDone)
    {
        initSensor();
    }

	readSensor();
	   
	MadgwickAHRSupdate();

	gyro_x.store(gx);
	gyro_y.store(gy);
	gyro_z.store(gz);
	
	//roll (x-axis rotation)
	double t0 = +2.0 * (q0 * q1 + q2 * q3);
	double t1 = 1.0 - 2.0 * (q1*q1 + q2*q2);
	roll.store((float)(atan2(t0, t1) * 180 / M_PI) - rollBias);
	
	//pitch (y-axis rotation)
	double t2 = -2.0 * (q0*q2 - q3*q1);
	t2 = ((t2 > 1.0) ? 1.0 : t2);
	t2 = ((t2 < -1.0) ? -1.0 : t2);
	pitch.store((float)(asin(t2) * 180 / M_PI) - pitchBias);

	//yaw (z-axis ratation)
	double t3 = +2.0 *(q0 * q3 + q1 * q2);
	double t4 = 1.0 - 2.0 * (q2*q2 + q3*q3);
	yaw.store((float)(atan2(t3, t4) * 180 / M_PI) - yawBias);
}

void IMUSensor::sensorWarmUp()
{
	us warmupTime{ 3 * 1000000 }; //3 seconds
	us calibTime{ 2 * 1000000 }; //2 seconds
	us startTime = getTimeU();
	float rollAvg = 0, pitchAvg = 0;
	int itterations = 0;
	while ((getTimeU() - startTime) < warmupTime)
	{
		while (!newSensorMeasurement)
		{
			newSensorMeasurement = false;
			usleep(500);
		}
		readSensor();
	}
	startTime = getTimeU();
	us then = getTimeU();
	us now;
	us proccessTime;
	us period{ (int)floor((1.0f / 2500 * 1000000)) }; //Time for each loop itteration
	while ((getTimeU() - startTime) < calibTime)
	{
		if (newSensorMeasurement)
		{
			rollAvg += roll.load();
			pitchAvg += pitch.load();
			itterations++;
			newSensorMeasurement = false;
			getAnglesMadgwick();
		}
		else
		{
			MadgwickAHRSupdate();
		}
		now = getTimeU();
		proccessTime = now - then;
		if (proccessTime < period)
		{
			std::this_thread::sleep_for(period - proccessTime);
		}
		then = getTimeU();
	}
	rollBias = rollAvg / (float)itterations;
	pitchBias = pitchAvg / (float)itterations;
}

void IMUSensor::calcCompensatedAcc()
{
	float gravx, gravy, gravz;
	gravx = 2 * (q1*q3 - q0*q2);
	gravy = 2 * (q0*q1 + q2*q3);
	gravz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
	accx_comp = ax - gravx;
	accy_comp = ay - gravy;
	accz_comp = az - gravz;
}

void IMUSensor::calcDynamicAcc()
{
	auto quatMult = [](float* q1, float* q2, float* res)
	{
		res[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
		res[1] = q1[0] * q2[1] + q1[1] * q2[0] - q1[2] * q2[3] + q1[3] * q2[2];
		res[2] = q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0] - q1[3] * q2[1];
		res[3] = q1[0] * q2[3] - q1[1] * q2[2] + q1[2] * q2[1] + q1[3] * q2[0];
		return;
	};

	calcCompensatedAcc();
	float q[4]{ q0,q1,q2,q3 };
	float q_conj[4]{ q0,-q1,-q2,-q3 };
	float acc[4]{ 0,accx_comp, accy_comp, accz_comp };
	float tmp[4];
	float res[4];
	quatMult(q, acc, tmp);
	quatMult(tmp, q_conj, res); //res = q*acc*q_conj
	accx_dyn.store(res[1]);
	accy_dyn.store(res[2]);
	accz_dyn.store(res[3]);
}

/////////////////////////////////////////////////////
//Private functions


void IMUSensor::MadgwickAHRSupdate()
{
	_now = getTimeU();
	int duration_U = (_now - _then).count();
	double dTime = (double)duration_U / 1000000.0;
	//std::cout << "dTime: " << dTime << std::endl;
	if ((lastDTime > 0.0001) && (dTime > 100 * lastDTime))
	{
		//std::cout << "using last time, dTime: " << dTime << ", last: " << lastDTime << std::endl;
		dTime = lastDTime;
	}
	else
	{
		//std::cout << "Using current time" << std::endl;
		lastDTime = dTime;
	}
	//std::cout << "Frequency: " << 1.0 / dTime << std::endl;
	_then = _now;

    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
    if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		MadgwickAHRSupdateIMU();
        return;
    }
	
    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Normalise magnetometer measurement
        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;
        // Auxiliary variables to avoid repeated arithmetic
        _2q0mx = 2.0f * q0 * mx;
        _2q0my = 2.0f * q0 * my;
        _2q0mz = 2.0f * q0 * mz;
        _2q1mx = 2.0f * q1 * mx;
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;
        _2q0q2 = 2.0f * q0 * q2;
        _2q2q3 = 2.0f * q2 * q3;
        q0q0 = q0 * q0;
        q0q1 = q0 * q1;
        q0q2 = q0 * q2;
        q0q3 = q0 * q3;
        q1q1 = q1 * q1;
        q1q2 = q1 * q2;
        q1q3 = q1 * q3;
        q2q2 = q2 * q2;
        q2q3 = q2 * q3;
        q3q3 = q3 * q3;
		

        // Reference direction of Earth's magnetic field
        hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
        _2bx = sqrtf(hx * hx + hy * hy);
        _2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
        _4bx = 2.0f * _2bx;
        _4bz = 2.0f * _2bz;

        // Gradient decent algorithm corrective step
        s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }
	
    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * dTime;
    q1 += qDot2 * dTime;
    q2 += qDot3 * dTime;
    q3 += qDot4 * dTime;

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

void IMUSensor::MadgwickAHRSupdateIMU()
{
	std::cout << "Using alt fusion algo!" << std::endl;
	_now = getTimeU();
	int duration_U = (_now - _then).count();
	float dTime = (float)duration_U / 1000000;
	if ((lastDTime > 0) && (dTime > 1000 * lastDTime))
	{
		dTime = lastDTime;
	}
	else
	{
		lastDTime = dTime;
	}
	_then = _now;

    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	
    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;
        _4q0 = 4.0f * q0;
        _4q1 = 4.0f * q1;
        _4q2 = 4.0f * q2;
        _8q1 = 8.0f * q1;
        _8q2 = 8.0f * q2;
        q0q0 = q0 * q0;
        q1q1 = q1 * q1;
        q2q2 = q2 * q2;
        q3q3 = q3 * q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * dTime;
    q1 += qDot2 * dTime;
    q2 += qDot3 * dTime;
    q3 += qDot4 * dTime;

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

float IMUSensor::invSqrt(float x)
{
    float y = 1.0f / sqrtf(x);
    return y;
}
