#ifndef _MOTOR_H_
#define _MOTOR_H_
#include <iostream>

//Class for motor object
class Motor
{
public:
//constructor and destructor
	Motor(int motorId = 0);
	~Motor() {};

	//getters
	int getMotorID()			{ return _motorId; };
	double getCurrentSpeedPWM() { return _currentSpeedPWM; };
	double getCurrentSpeedNum() { return _currentSpeedNum; };
	bool getArmStatus()			{ return _armed; };
	
	//setters
	void setCurrentSpeedNum(double speed) { _currentSpeedNum = speed; };
	void setCurrentSpeedPWM(double speed) { _currentSpeedPWM = speed; };
	void setArmed(bool armed)						  { _armed = armed; };

private:
	//variables
	int _motorId;		//Number of motor to identify
	double _currentSpeedNum;	//Current speed of motor in %
	double _currentSpeedPWM;	//Current speed in PWM on ticks
	bool _armed = false;
};

#endif // !_MOTOR_H_


