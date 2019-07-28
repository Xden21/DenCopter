#ifndef _MOTOR_DRIVER_TYPES_H_
#define _MOTOR_DRIVER_TYPES_H_

#include "Motor_Driver_Registers.h"

struct Mode1Settings
{
	//in msb order
	uint8_t restart;
	uint8_t auto_increment_enable;
	uint8_t sleep_enable;
	uint8_t sub1_enable;
	uint8_t sub2_enable;
	uint8_t sub3_enable;
	uint8_t allcall_enable;
};



struct Motor_Driver_Settings
{
	Mode1Settings mode1;
};

#endif // !_MOTOR_DRIVER_TYPES_H_
