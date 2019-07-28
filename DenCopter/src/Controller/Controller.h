#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

/********************************
Stabilizer class with functions to run to hold the quadcopter steady in the air.
Uses PID control theory to get a stable algorithm.

HEAVY WIP!!!
********************************/



enum ANGLE
{
	ROLL = 0,
	PITCH = 1,
	YAW = 2,
	THROTTLE = 3
};

enum ALTITUDE_PID
{
	HEIGHT = 0,
	VELOCITY = 1
};

//imports
#include "../Motors/Motor_Driver.h"
#include "../Sensors/IMUSensor.h"
#include "PID.h"
#include "../Tools/Timer.h"
#include "../Tools/ConfigHandler.h"
#include "../Sensors/BMP280.h"
#include "altitude_kf.h"
#include "../Domain/PIDData.h"
#include <algorithm>
#include <iostream>

//For multithreading
#include <thread>
#include <atomic>

//CONSTANTS
#define PID_AMOUNT 8
#define RATE_LOOP_FREQ 250
#define ANGLE_LOOP_FREQ 2500
#define BARO_READ_SPEED 30
#define THROTTLE_DEADZONE 20
#define MAX_VELOCITY 3.0f    //m/s

class Controller
{
public:
	std::mutex PIDMutex;

	enum MOTORS
	{
		MOTORFL = 1,
		MOTORFR = 0,
		MOTORBL = 2,
		MOTORBR = 3
	};

	enum PID_CONTROLLER
	{
		ROLL_RATE = 0,
		PITCH_RATE = 1,
		YAW_RATE = 2,
		ALTITUDE_SPEED = 3,
		ALTITUDE_HEIGHT = 4,
		ROLL_ANGLE = 5,
		PITCH_ANGLE = 6,
		YAW_ANGLE = 7
	};

	enum ALTITUDE_MODE
	{
		FREE_MODE = 0,
		ALT_STABILIZE = 1,
		ALT_HOLD = 2
	};



	Controller(Motor_Driver* driver_object, IMUSensor* sensor_object, BMP280* altitude_sensor_object, Motor** motors);  //Default constructor possible but then you can't access the sensor and driver from outside the class
	~Controller();

	//Public functions
	void initController();
	void armMotors();
	void disarmMotors();
	void sensorWarmUp();
	void setLanded(bool landed_args);
	bool getLanded();
	void setAngleMode(bool angleMode_arg);
	bool getAngleMode();
	void setInput(float* inputs, int altMode, bool killSwitch);
	void getAttitudeData(float* data);


	//TEMP

	float getThrottleSetpoint();

	//PID Outputs
	std::atomic<float> attitudeOutput[3] = {};
	std::atomic<float> altitudeOutput[2] = {};

	//Conf settings
	//Throttle
	float throttle_var, lift_point;

	//Rate PIDS
	float yaw_rate_KP, yaw_rate_KD, yaw_rate_KI;
	float pitch_rate_KP, pitch_rate_KD, pitch_rate_KI;
	float roll_rate_KP, roll_rate_KD, roll_rate_KI;
	float max_yaw_rate, max_pitch_rate, max_roll_rate;

	// Attitude maxima
	float max_yaw_angle, max_pitch_angle, max_roll_angle;

	//Altitude PIDS
	float velocity_KP, velocity_KD, velocity_KI;
	float height_KP, height_KD, height_KI;

	// Angle PIDS
	float pitch_angle_KP, pitch_angle_KD, pitch_angle_KI;
	float roll_angle_KP, roll_angle_KD, roll_angle_KI;
	float yaw_angle_KP, yaw_angle_KD, yaw_angle_KI;

	PID pids[PID_AMOUNT];

	void updateRatePids(Attitude_PIDData* data);
	void updateAnglePids(Attitude_PIDData* data);
private:
	/////////////////////////////////////////////////////////////////////////
	//Config file name
	static const std::string confFileName;
	ConfigHandler* confFile;



	//Functions
	void readConfig();
	void writeConfig();

	void updateMotors();


	//////////////////////////////////////////////////////////////////////////
	//PID functions
	void setSetpoints(float* inputs, int altmode);

	void setThrottleSetpoint(float setpoint);
	void setHeightSetpoint(PID_CONTROLLER controller, float setpoint);
	void setRateSetpoint(PID_CONTROLLER controller, float setpoint);
	void setAngleSetpoint(PID_CONTROLLER controller, float setpoint);

	void setAltitudeSetpoints(float throttle);


	float getRateSetpoint(PID_CONTROLLER controller);
	float getAngleSetpoint(PID_CONTROLLER controller);
	float getHeightSetpoint(PID_CONTROLLER controller);

	void getRateControlValue(PID_CONTROLLER controller, ANGLE angle);

	void getAngleControlValue(PID_CONTROLLER controller, ANGLE angle);

	void getHeightControlValue(PID_CONTROLLER controller, ALTITUDE_PID pid);

	void getMotorSpeeds();


	void setupRatePids();
	void setupAnglePids();
	void setupAltitudePids();
	//void updateAltPids();

	//////////////////////////////////////////////////////////////////////////
	//Thread functs
	void rateThread();
	void angleThread();
	bool useBaro();
	void altitudeHandler();
	void attitudeHandler();
	void startRateThread();
	void startAngleThread();
	void finishThreads();



	//////////////////////////////////////////////////////////////////////////
	//Private variables
	Motor_Driver* driver;
	IMUSensor* sensor;
	Motor* motorFL;
	Motor* motorFR;
	Motor* motorBL;
	Motor* motorBR;
	BMP280* altitude_sensor;					//pids: Roll rate, Pitch rate, Yaw rate, Roll angle, Pitch angle, Yaw angle, Throttle

	Altitude_KF* altitude_filter;

	std::thread RatePIDThread;
	std::thread anglePIDThread;

	//Variables used only in main thread
	float throttleSetpoint = 0;
	float motorSpeeds[4] = { 0,0,0,0 };
	std::atomic<bool> angleMode{ false };
	std::atomic<bool> altHoldSet{ false };

	std::atomic<bool> killSwitch{ true };

	//Variables used commonly between threads
	
	std::atomic<bool> landed{ true };				//When true: stop the pid controller threads, when false: start the pid controller threads
	std::atomic<bool> altHold{ false };				//When true: start altitude hold mode
	std::atomic<bool> altStabilize{ false };		//When true: start altitude stabilize mode

	us lastBaroRead = (us)0;
};
#endif