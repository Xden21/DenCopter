#pragma once

struct Attitude_PIDData
{
	float yaw_KP;
	float yaw_KI;
	float yaw_KD;
	float pitch_KP;
	float pitch_KI;
	float pitch_KD;
	float roll_KP;
	float roll_KI;
	float roll_KD;
};