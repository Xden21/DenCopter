#include "Motor.h"


Motor::Motor(int motorId)
{
	switch (motorId)
	{
	case 1:
		_motorId = motorId - 1;
		break;
	case 2:
		_motorId = motorId - 1;
		break;
	case 3:
		_motorId = 11 + motorId;
		break;
	case 4:
		_motorId = 11 + motorId;
		break;
	default:
		std::cout << "MotorPinNotFound" << std::endl;;
	}

}
