#include "Controller.h"

// Set the values from the config header file to the Rate PID controllers.
void Controller::setupRatePids()
{
	pids[YAW_RATE].setGain(KP, yaw_rate_KP);
	pids[YAW_RATE].setGain(KD, yaw_rate_KD);
	pids[YAW_RATE].setGain(KI, yaw_rate_KI);
	pids[YAW_RATE].setOutputBoundaries(-30, 30);

	pids[PITCH_RATE].setGain(KP, pitch_rate_KP);
	pids[PITCH_RATE].setGain(KD, pitch_rate_KD);
	pids[PITCH_RATE].setGain(KI, pitch_rate_KI);
	pids[PITCH_RATE].setOutputBoundaries(-50, 50);

	pids[ROLL_RATE].setGain(KP, roll_rate_KP);
	pids[ROLL_RATE].setGain(KD, roll_rate_KD);
	pids[ROLL_RATE].setGain(KI, roll_rate_KI);
	pids[ROLL_RATE].setOutputBoundaries(-50, 50);
}

// Set the values from the config file to the Altitude PID controllers.
void Controller::setupAltitudePids()
{
	pids[ALTITUDE_SPEED].setGain(KP, velocity_KP);
	pids[ALTITUDE_SPEED].setGain(KD, velocity_KD);
	pids[ALTITUDE_SPEED].setGain(KI, velocity_KI);
	pids[ALTITUDE_SPEED].setOutputBoundaries(-50, 50);

	pids[ALTITUDE_HEIGHT].setGain(KP, height_KP);
	pids[ALTITUDE_HEIGHT].setGain(KD, height_KD);
	pids[ALTITUDE_HEIGHT].setGain(KI, height_KI);
	pids[ALTITUDE_HEIGHT].setOutputBoundaries(-MAX_VELOCITY, MAX_VELOCITY); //In m/s
}

void Controller::setupAnglePids()
{
	pids[YAW_RATE].setGain(KP, yaw_rate_KP);
	pids[YAW_RATE].setGain(KD, yaw_rate_KD);
	pids[YAW_RATE].setGain(KI, yaw_rate_KI);
	pids[YAW_RATE].setOutputBoundaries(-40, 40);

	pids[PITCH_ANGLE].setGain(KP, pitch_angle_KP);
	pids[PITCH_ANGLE].setGain(KD, pitch_angle_KD);
	pids[PITCH_ANGLE].setGain(KI, pitch_angle_KI);
	pids[PITCH_ANGLE].setOutputBoundaries(-65, 65);

	pids[ROLL_ANGLE].setGain(KP, roll_angle_KP);
	pids[ROLL_ANGLE].setGain(KD, roll_angle_KD);
	pids[ROLL_ANGLE].setGain(KI, roll_angle_KI);
	pids[ROLL_ANGLE].setOutputBoundaries(-65, 65);
}

void Controller::updateRatePids(Attitude_PIDData* data)
{
	pids[YAW_RATE].setGain(KP, data->yaw_KP);
	pids[YAW_RATE].setGain(KI, data->yaw_KI);
	pids[YAW_RATE].setGain(KD, data->yaw_KD);
	
	pids[PITCH_RATE].setGain(KP, data->pitch_KP);
	pids[PITCH_RATE].setGain(KI, data->pitch_KI);
	pids[PITCH_RATE].setGain(KD, data->pitch_KD);

	pids[ROLL_RATE].setGain(KP, data->roll_KP);
	pids[ROLL_RATE].setGain(KI, data->roll_KI);
	pids[ROLL_RATE].setGain(KD, data->roll_KD);
}


void Controller::updateAnglePids(Attitude_PIDData* data)
{
	pids[YAW_ANGLE].setGain(KP, data->yaw_KP);
	pids[YAW_ANGLE].setGain(KI, data->yaw_KI);
	pids[YAW_ANGLE].setGain(KD, data->yaw_KD);

	pids[PITCH_ANGLE].setGain(KP, data->pitch_KP);
	pids[PITCH_ANGLE].setGain(KI, data->pitch_KI);
	pids[PITCH_ANGLE].setGain(KD, data->pitch_KD);

	pids[ROLL_ANGLE].setGain(KP, data->roll_KP);
	pids[ROLL_ANGLE].setGain(KI, data->roll_KI);
	pids[ROLL_ANGLE].setGain(KD, data->roll_KD);
}


// Interpret the inputs according to the flight mode and use those to set the setpoints for the PID controllers.
void Controller::setSetpoints(float* inputs, int altmode)
{
	if (getAngleMode())
	{
		setAngleSetpoint(ROLL_ANGLE, (inputs[ROLL] * (max_roll_angle * 2.0f / 100)) - max_roll_angle);
		setAngleSetpoint(PITCH_ANGLE, (inputs[PITCH] * (max_pitch_angle * 2.0f / 100)) - max_pitch_angle);
		setRateSetpoint(YAW_RATE, (inputs[YAW] * (max_yaw_rate * 2.0f / 100)) - max_yaw_rate);

		switch (altmode)
		{
		case FREE_MODE:
		{
			altHold.store(false);
			altStabilize.store(false);
			break;
		}
		case ALT_STABILIZE:
		{
			altHold.store(false);
			altStabilize.store(true);
			break;
		}
		case ALT_HOLD:
		{
			altHold.store(true);
			altStabilize.store(false);
			break;
		}
		}
		setAltitudeSetpoints((inputs[THROTTLE] * 2) - 100);
	}
	else
	{
		setRateSetpoint(ROLL_RATE, (inputs[ROLL] * (max_roll_rate * 2.0f / 100)) - max_roll_rate);
		setRateSetpoint(PITCH_RATE, (inputs[PITCH] * (max_pitch_rate * 2.0f / 100)) - max_pitch_rate);
		setRateSetpoint(YAW_RATE, (inputs[YAW] * (max_yaw_rate * 2.0f / 100)) - max_yaw_rate);
		altHold.store(false);
		altStabilize.store(false);
		setAltitudeSetpoints((inputs[THROTTLE] * 2) - 100);
	}
}

//Setpoint getters and setters
void Controller::setThrottleSetpoint(float setpoint)
{
	throttleSetpoint = setpoint;
}

float Controller::getThrottleSetpoint()
{
	return throttleSetpoint;
}

void Controller::setRateSetpoint(PID_CONTROLLER controller, float setpoint)
{
	pids[controller].setSetpoint(setpoint);
}

float Controller::getRateSetpoint(PID_CONTROLLER controller)
{
	return pids[controller].getSetpoint();
}

void Controller::setAngleSetpoint(PID_CONTROLLER controller, float setpoint)
{
	pids[controller].setSetpoint(setpoint);
}

float Controller::getAngleSetpoint(PID_CONTROLLER controller)
{
	return pids[controller].getSetpoint();
}

void Controller::setHeightSetpoint(PID_CONTROLLER controller, float setpoint)
{
	pids[controller].setSetpoint(setpoint);
}

float Controller::getHeightSetpoint(PID_CONTROLLER controller)
{
	return pids[controller].getSetpoint();
}

void Controller::setAltitudeSetpoints(float throttle)
{
	if (altStabilize.load())
	{
		float velocity;
		if (fabs(throttle) < THROTTLE_DEADZONE)
		{
			velocity = 0.0f;
		}
		else
		{
			velocity = ((throttle - THROTTLE_DEADZONE) / (100 - THROTTLE_DEADZONE)) * (MAX_VELOCITY / 2);
		}
		setHeightSetpoint(ALTITUDE_SPEED, velocity);
		altHoldSet = false;
		return;
	}
	else if (altHold.load() && !altHoldSet)
	{
		setHeightSetpoint(ALTITUDE_HEIGHT, altitude_filter->getHeight());
		altHoldSet = true;
		return;
	}
	else if (altHold.load())
	{
		return;
	}
	else //FREE MODE
	{
		throttle /= 100.0f;
		setThrottleSetpoint(lift_point + (throttle * throttle_var)); //Work around liftpoint
	}
}
// Evaluate the Rate PID algortithm for the desired direction.
void Controller::getRateControlValue(PID_CONTROLLER controller, ANGLE angle)
{
	std::unique_lock<std::mutex> PIDLock(PIDMutex);
	float error = 0;
	float input = 0;
	switch (angle)
	{
	case YAW:
	{
		input = sensor->gyro_z.load();
		error = getRateSetpoint(controller) - input;
		break;
	}
	case PITCH:
	{
		input = sensor->gyro_y.load();
		error = getRateSetpoint(controller) - input;
	}
	case ROLL:
	{
		input = sensor->gyro_x.load();
		error = getRateSetpoint(controller) - input;
	}
	}
	attitudeOutput[angle].store(pids[controller].calcPID(error, input));
}

// Evaluate the Angle PID algorithm for the desired direction.

void Controller::getAngleControlValue(PID_CONTROLLER controller, ANGLE angle)
{
	std::unique_lock<std::mutex> PIDLock(PIDMutex);

	float error = 0;
	float input = 0;
	switch (angle)
	{
	case YAW:
	{
		input = sensor->yaw.load();
		error = getAngleSetpoint(controller) - input;
		break;
	}
	case PITCH:
	{
		input = sensor->pitch.load();
		error = getAngleSetpoint(controller) - input;
		break;
	}
	case ROLL:
	{
		input = sensor->roll.load();
		error = getAngleSetpoint(controller) - input;
		break;
	}
	}
	attitudeOutput[angle].store(pids[controller].calcPID(error, input));
}

void Controller::getHeightControlValue(PID_CONTROLLER controller, ALTITUDE_PID pid)
{
	std::unique_lock<std::mutex> PIDLock(PIDMutex);

	float error = 0;
	float input = 0;
	switch (pid)
	{
	case HEIGHT:
	{
		input = altitude_filter->getHeight();
		error = getHeightSetpoint(controller) - input;
		break;
	}
	case VELOCITY:
	{
		input = altitude_filter->getVelocity();
		error = getHeightSetpoint(controller) - input;
		break;
	}
	}
	altitudeOutput[pid].store(pids[controller].calcPID(error, input));
}

// Calculate the motor speeds from the outputs of the Rate PID Controllers.
// The outputs are normalised between 0 and 100, if one speed was outside of these bounds
// all outputs will be resized proportionally.
void Controller::getMotorSpeeds()
{
	float throttle = getThrottleSetpoint();
	//std::cout << "Throttle: " << throttle << std::endl;
	//Set motorspeeds with values of PIDS
	motorSpeeds[MOTORFL] = throttle + attitudeOutput[PITCH] + attitudeOutput[ROLL] + attitudeOutput[YAW];
	motorSpeeds[MOTORFR] = throttle + attitudeOutput[PITCH] - attitudeOutput[ROLL] - attitudeOutput[YAW];
	motorSpeeds[MOTORBL] = throttle - attitudeOutput[PITCH] + attitudeOutput[ROLL] - attitudeOutput[YAW];
	motorSpeeds[MOTORBR] = throttle - attitudeOutput[PITCH] - attitudeOutput[ROLL] + attitudeOutput[YAW];

	//Constrain motorspeeds within bounds
	float min_speed = motorSpeeds[0];
	float max_speed = motorSpeeds[0];

	for (int i = 1; i < 4; i++)
	{
		if (motorSpeeds[i] < min_speed)
		{
			min_speed = motorSpeeds[i];
		}
		else if (motorSpeeds[i] > max_speed)
		{
			max_speed = motorSpeeds[i];
		}
	}
	float correction_min = 0;
	float correction_max = 0;

	if (min_speed < 0)
	{
		correction_min = -min_speed;
	}
	if (max_speed > 100)
	{
		correction_max = 100 - max_speed;
	}

	for (int i = 0; i < 4; i++)
	{
		motorSpeeds[i] = motorSpeeds[i] + correction_min + correction_max;
	}
}