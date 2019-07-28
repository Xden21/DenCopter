#include "Controller.h"

// Function for the Rate mode, runs every measurement (250Hz).
// Gets gyro data from sensor, and calculates the PID values for every angle.
// No alt hold in rate control.
void Controller::rateThread()
{
	auto then = getTimeU();
	us now;
	us proccessTime;
	us period{ (int)floor((1.0f / RATE_LOOP_FREQ * 1000000)) }; //Time for each loop itteration
	while (!getLanded() && newSensorMeasurement)
	{
		newSensorMeasurement = false;
		sensor->getGyro();
		getRateControlValue(ROLL_RATE, ROLL);
		getRateControlValue(PITCH_RATE, PITCH);
		getRateControlValue(YAW_RATE, YAW);
		updateMotors();
		now = getTimeU();
		proccessTime = now - then;
		if (proccessTime < period)
		{
			std::this_thread::sleep_for(period - proccessTime);
		}
		then = getTimeU();
	}
}

// Function for the Angle mode, tries to run at 2500Hz, but not each time with new data.
// If no new data available it itterates over the previous angle, it tries to read at 250Hz.
// Gets angle data from sensor, and calculates the PID values for every angle.
// Then calculates the rate PID values.
void Controller::angleThread()
{
	us period{ (int)floor((1.0f / ANGLE_LOOP_FREQ * 1000000)) }; //Time for each loop itteration
	//std::cout << "Period: " << (int)period.count() << std::endl;
	auto then = getTimeU();
	us diff;
	us now;
	us proccessTime;
	while (!getLanded())
	{		
		if (newSensorMeasurement)
		{
			newSensorMeasurement = false;

			// Apply control action for previous measurement since we now have iterated some 10 times and so should have converged
			// TODO late or not?
			attitudeHandler();
			updateMotors();
			std::cout << "Reading Sensor " << std::endl;
			sensor->getAnglesMadgwick();
			if (useBaro())
			{
				std::cout << "Using Baro" << std::endl;
				sensor->calcDynamicAcc();
				altitude_filter->propagate(sensor->accz_dyn);
				altitudeHandler();
			}
		}
		else
		{
			sensor->MadgwickAHRSupdate();
		}	

		now = getTimeU();
		proccessTime = now - then;
		//std::cout << "proccessTime: " << (int)proccessTime.count() << std::endl;
		if (proccessTime < period)
		{
			//std::cout << "sleepTime: " << (int)sleepTime.count() << std::endl;
			std::this_thread::sleep_for(period - proccessTime);
		}
		then = getTimeU();
	}
	std::cout << "Finishing thread" << std::endl;
}

void Controller::attitudeHandler()
{
	getRateControlValue(YAW_RATE, YAW);
	getAngleControlValue(PITCH_ANGLE, PITCH);
	getAngleControlValue(ROLL_ANGLE, ROLL);
}

void Controller::altitudeHandler()
{
	us now = getTimeU();
	us readTime{ (int)floor((1.0f / BARO_READ_SPEED * 1000000)) };
	if ((now - lastBaroRead).count() >= readTime.count())
	{
		altitude_filter->update(altitude_sensor->getAltitude());
		lastBaroRead = getTimeU();
	}
	if (altHold.load())
	{
		// TODO one PID (easier to tune)?
		getHeightControlValue(ALTITUDE_HEIGHT, HEIGHT);
		setHeightSetpoint(ALTITUDE_SPEED, altitudeOutput[HEIGHT].load());
		getHeightControlValue(ALTITUDE_SPEED, VELOCITY);
		setThrottleSetpoint(altitudeOutput[VELOCITY].load());
	}
	else
	{
		getHeightControlValue(ALTITUDE_SPEED, VELOCITY);
		setThrottleSetpoint(altitudeOutput[VELOCITY].load());
	}
}

// Start the rate thread
void Controller::startRateThread()
{
	RatePIDThread = std::thread(&Controller::rateThread, this);
}

// Start the angle thread
void Controller::startAngleThread()
{
	anglePIDThread = std::thread(&Controller::angleThread, this);
}

// Finish the threads
void Controller::finishThreads()
{

	if (angleMode)
	{
		anglePIDThread.join();
	}
	else
	{
		RatePIDThread.join();
	}
}