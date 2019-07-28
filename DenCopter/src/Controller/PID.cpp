#include "PID.h"

// Set initial values to zero.
PID::PID()
{
	Kd = 0;
	Ki = 0;
	Kp = 0;
	integral = 0;
	prev_der = 0;
	prev_input = 0;
	prev_time = (us)0;
	setpoint = 0;
	last_output = 0;
}

// Initialise the tuning vars.
void PID::initPID(float kp, float ki, float kd, float i_minOutput, float i_maxOutput)
{
	Kd = kp;
	Ki = ki;
	Kp = kd;
	minOutput = i_minOutput;
	maxOutput = i_maxOutput;
	prev_time = getTimeU();
}

// Calculate the output of the PID algortihm.
float PID::calcPID(float error, float input)
{
	us now = getTimeU();
	int delta_usec = (now - prev_time).count();

	float output = 0;
	
	if (prev_time == (us)0 || delta_usec > MAX_DELTA_TIME)
	{
		prev_input = input;
		integral = last_output;
		delta_usec = 0;
	}

	prev_time = now;

	//Proportional gain (always)
	output += Kp * error;
	//std::cout << "output: "<< output << std::endl;
	
	//Make sure time has passed for time sensitive components
	if (delta_usec > 500)
	{
		float dTime = (float)delta_usec / 1e6f;		//in seconds

		if (Ki > 0)
		{
			//intergral
			integral += Ki * dTime * error;

			if (integral < -3 / 4 * minOutput)
			{
				integral = -3 / 4 * minOutput;
			}
			else if (integral > 3 / 4 * maxOutput)
			{
				integral = 3 / 4 * maxOutput;
			}
			output += integral;
		}
		if (Kd > 0)
		{
			float derivative = (input - prev_input) / dTime;

			derivative = DIFF_FILTER * derivative + (1 - DIFF_FILTER) * prev_der;

			prev_der = derivative;
			prev_input = input;

			output -= Kd * derivative;
		}
	}
	if (output < minOutput)
	{
		output = minOutput;
	}
	else if (output > maxOutput)
	{
		output = maxOutput;
	}
	last_output = output;
	return output;
}

// Set tuning vars. (following functions)

void PID::setGain(GAIN_TYPES type, float gain)
{
	switch (type)
	{
	case KP:
		Kp = gain;
		break;
	case KI:
		Ki = gain;
		break;
	case KD:
		Kd = gain;
		break;
	}
}

void PID::setSetpoint(float setpoint_value)
{
	setpoint = setpoint_value;
}

void PID::setOutputBoundaries(float i_outputMin, float i_outputMax)
{
	minOutput = i_outputMin;
	maxOutput = i_outputMax;
}

// Get the gain values.
float PID::getGain(GAIN_TYPES type)
{
	switch (type)
	{
	case KP:
	{
		return Kp;
		break;
	}
	case KI:
	{
		return Ki;
		break;
	}
	case KD:
	{
		return Kd;
		break;
	}
	}
}
