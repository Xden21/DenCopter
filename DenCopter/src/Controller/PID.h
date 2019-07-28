#ifndef _PID_CONTROLLERS_H_
#define _PID_CONTROLLERS_H_

#include "../Tools/Timer.h"

//CONSTANTS
#define DIFF_FILTER 0.5f
#define MAX_DELTA_TIME 500000

enum GAIN_TYPES
{
	KP,
	KI,
	KD
};


//PID class and general calculation functions

//struct with specific values for a PID controller
class PID
{
public:
	//Constructor and Destructor
	PID();

	~PID() {};

	void initPID(float kp, float ki, float kd, float i_minOutput, float i_maxOutput);

	float calcPID(float error, float input);

	void setGain(GAIN_TYPES type, float gain);

	void setSetpoint(float setpoint_value);

	void setOutputBoundaries(float i_outputMin,float i_outputMax);

	float getGain(GAIN_TYPES type);

	float getSetpoint() { return setpoint; }

private:
	float Kp;
	float Kd;
	float Ki;
	float minOutput;
	float maxOutput;

	us prev_time;

	float prev_der;
	float prev_input;
	float integral;
	float last_output;

	float setpoint;

};

#endif // !_PID_CONTROLLERS_H_
