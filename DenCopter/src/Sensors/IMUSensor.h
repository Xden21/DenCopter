//
// IMU Sensor class to initialize and read the sensorpackage, and convert it to usable data.
//

#ifndef QUADCOPTER_MADGWICHAHRS_H
#define QUADCOPTER_MADGWICHAHRS_H

#include "MPU9250.h"
#include <cmath>
#include <chrono>
#include <algorithm>
#include <atomic>
#include <iostream>
#include "../Tools/Timer.h"

#define MANUAL_CALIBRATION	true
#define GRAVITANIONAL_ACCELERATION 9.81142f
#define INT_PIN 4 // TODO

// Global variable for new sensor variable.
extern volatile bool newSensorMeasurement;

class IMUSensor: public MPU9250
{
public:

    //Variables
    volatile float beta;        //algotithm gain

	std::atomic<float> roll;                 //x-axis rotation
    std::atomic<float> pitch;                //y-axis rotation
    std::atomic<float> yaw;                  //z-axis rotation

	std::atomic<float> gyro_x;
	std::atomic<float> gyro_y;
	std::atomic<float> gyro_z;

	std::atomic<float> accx_dyn, accy_dyn, accz_dyn;	   //Dynamic acceleration aligned with earth frame

	float rollBias = 0;
	float pitchBias = 0;
	float yawBias = 1.05; // Exact for this location

    bool initDone = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Functions
    //Get the roll - pitch - yaw angles of the drone
    void initSensor();
	void sensorWarmUp();
	void MadgwickAHRSupdate();
    void getAnglesMadgwick();
	void getGyro();

	//Accelleration calculations.
	void calcDynamicAcc();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Constructor
    explicit IMUSensor(float beta = 0.1f, bool manual_calib = false)
            :beta{beta}, q0{1.0f}, q1{0.0f}, q2{0.0f}, q3{0.0f} , roll{0.0f}, pitch{0.0f}, yaw{0.0f}, MPU9250(manual_calib)
    {
		initSensor();
    }

	//TEMP
	
	float accx_comp, accy_comp, accz_comp; //Gravity compensated acceleration values in g
	

private:
    //Variables
    float _sampleFreq;
    us _now;
    us _then;
	double lastDTime;

	volatile float q0, q1, q2, q3;
	

    //Functions
    float invSqrt(float x);
	
    void MadgwickAHRSupdateIMU();

	void calcCompensatedAcc();	
};

#endif //QUADCOPTER_MADGWICHAHRS_H
