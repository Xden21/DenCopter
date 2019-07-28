#ifndef _Motor_Driver_H_
#define _Motor_Driver_H_

//Class that sets up the Adafruit 16ch servo hat and sets it to drive the motors
//Includes motor controls and pwm frequency control


//Imports
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <limits>
#include <cmath>
#include <atomic>
#include "Motor_Driver_Registers.h"
#include "Motor_Driver_Types.h"
#include "Motor.h"
#include "../Comm/I2Ccom.h"

#define TICK_RANGE 4096
#define DUTY_MIN 50
#define DUTY_MAX 92

class Motor_Driver
{
public:
	Motor_Driver_Settings settings;


	Motor_Driver(int freq = 50);
	~Motor_Driver();

	//Called by constructor, can be recalled to change settings
	void init();
	void begin();

	//Control functions
	void setMotor(Motor &motor, float speed, bool armoveride = false);			//speed in %
	void armMotor(Motor &motor);
	void disarmMotor(Motor &motor);
	void stopMotor(Motor &motor);
	void setPWM_freq(int freq);													//in Hz


private:
	//Variables
	float _freq;
	bool _init = false;
	int fileDescriptorMOT;
};

#endif // !_MOTOR_DRIVER_H_