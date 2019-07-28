
#include "Sensors/IMUSensor.h"
#include "Motors/Motor_Driver.h"
#include "Motors/Motor.h"
#include "Comm/PCInput.h"
#include "Comm/RXInput.h"
#include "Controller/Controller.h"
#include "Tools/Timer.h"
#include "Display/Display.h"
#include "Tools/ESC_Programming_Mode.h"
#include "Wireless_Logging/WirelessLogging.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>

#define MOTOR_REFRESH_FREQ 250
#define MOTOR_DRIVER_FREQ 400
#define SENSOR_BETA 1.0f
#define SINGLE_ANGLE_PID_MODE true

volatile bool newSensorMeasurement = true;

//Main loop for the the Quad, syncs input with the controller.
void StartCopter()
{
	//Start timer
	startTimer();

	//Init display
	Display display;

	//Used to init controller, initted here for debugging purpose, all operations should be done with controller
	Motor motorFL(2);
	Motor motorFR(1);
	Motor motorBL(3);
	Motor motorBR(4);
	Motor_Driver driver(MOTOR_DRIVER_FREQ);

	IMUSensor sensor(SENSOR_BETA, true);
	Motor* motors[4] = { &motorFL, &motorFR, &motorBL, &motorBR };
	BMP280 altitude_sensor;

	//Init controller
	Controller controller(&driver, &sensor, &altitude_sensor, motors);
	//Init input
	RXInput input;
	input.start_RX_Input();

	controller.setAngleMode(input.getAngleMode());

	WirelessLogging logger(&controller);

	float loggerdata[6] = { 0,0,0,0,0,0 };
	logger.startLogging(loggerdata);

	std::cout << "READY" << std::endl;
	display.writeStringToDisp("RDY");
	bool armed = false;
	us period{ (int)floor((1.0f / MOTOR_REFRESH_FREQ * 1000000)) };
	while (!input.getShutdown())
	{
		controller.setAngleMode(input.getAngleMode());
		if (input.getArmed() && (!armed) && !input.getKillSwitch())
		{
			std::cout << "Arming..." << std::endl;
			display.writeStringToDisp("CAL.B", true);
			armed = true;
			controller.armMotors();
			controller.sensorWarmUp();
			controller.setLanded(false);
			display.writeStringToDisp("ARM");
		}
		else if ((!input.getArmed() & armed) || input.getKillSwitch())
		{
			std::cout << "Disarming..." << std::endl;
			display.writeStringToDisp("D.ARM", true);
			armed = false;
			controller.disarmMotors();
			controller.setLanded(true);
		}
		//Start input to output loop
		us then = getTimeU();
		float inputs[4] = {};
		while (input.getArmed())
		{
			input.getInputs(inputs); //Get inputs from controller
			controller.setInput(inputs, input.getAltMode(), input.getKillSwitch());

			controller.getAttitudeData(loggerdata);
			loggerdata[3] = inputs[2];
			loggerdata[4] = inputs[1];
			loggerdata[5] = inputs[0];

			logger.setLoggingData(loggerdata);

			auto now = getTimeU();
			auto proccessTime = now - then;
			if (proccessTime < period)
			{
				std::this_thread::sleep_for(period - proccessTime);
			}
			then = getTimeU();
		}
		delay(10);
	}
	std::cout << "exiting" << std::endl;
	return;
}

//Test function with debugfile output.
void testCopter(std::string filename)
{
	startTimer();

	Display display;
	std::fstream debugFile;
	debugFile.open(filename, std::ios::out | std::ios::trunc);
	debugFile << "ANGLE TEST\n";
	debugFile << "Roll input;Roll Setpoint; Roll Angle;Roll Angle PID;Front Left Motor; ;Pitch Input;Pitch Setpoint;Pitch Angle;Pitch Angle PID;Front Right Motor; ;Yaw Input;Yaw Rate; Yaw Rate PID;Back Left Motor; ;Throttle;Back Right Motor\n";
	
	Motor motorFL(2);
	Motor motorFR(1);
	Motor motorBL(3);
	Motor motorBR(4);
	Motor_Driver driver(MOTOR_DRIVER_FREQ);
	IMUSensor sensor(SENSOR_BETA);
	Motor* motors[4] = { &motorFL, &motorFR, &motorBL, &motorBR };
	BMP280 altitude_sensor;
	bool armed = false;
	RXInput input;
	Controller controller(&driver, &sensor, &altitude_sensor, motors);
	input.start_RX_Input();
	sleep(3);
	std::cout << "READY" << std::endl;
	display.writeStringToDisp("RDY");
	us period{ (int)floor((1.0f / MOTOR_REFRESH_FREQ * 1000000)) };
	try {
		while (!input.getShutdown())
		{
			controller.setAngleMode(true);
			if (input.getArmed() & !armed && !input.getKillSwitch())
			{
				std::cout << "Arming..." << std::endl;
				display.writeStringToDisp("CAL.B", true);
				armed = true;
				controller.armMotors();
				controller.sensorWarmUp();
				controller.setLanded(false);
				display.writeStringToDisp("ARM");
			}
			
			else if ((!input.getArmed() & armed) || input.getKillSwitch())
			{
				std::cout << "Disarming..." << std::endl;
				display.writeStringToDisp("D.ARM",true);
				armed = false;
				controller.disarmMotors();
				controller.setLanded(true);
			}
			
			us then = getTimeU();
			float inputs[4] = {};
			while (input.getArmed())
			{
				//std::cout << "In MotorLoop" << std::endl;;				
				input.getInputs(inputs);
				controller.setInput(inputs, input.getAltMode(), input.getKillSwitch());

				//Data logging				
				debugFile << (inputs[0] * (controller.max_roll_angle*2.0f / 100)) - controller.max_roll_angle << ";" << controller.pids[3].getSetpoint() << ";" << sensor.roll.load() << ";" << controller.attitudeOutput[ROLL] << ";" << motorFL.getCurrentSpeedNum() << ";" ";";
				debugFile << (inputs[1] * (controller.max_pitch_angle*2.0f / 100)) - controller.max_pitch_angle << ";" << controller.pids[4].getSetpoint() << ";" << sensor.pitch.load() << ";" << controller.attitudeOutput[PITCH] << ";" << motorFR.getCurrentSpeedNum() << ";" ";";
				debugFile << (inputs[2] * (controller.max_yaw_rate*2.0f / 100)) - controller.max_yaw_rate << ";" << controller.pids[2].getSetpoint() << ";" << sensor.gz << ";" << controller.attitudeOutput[YAW] << ";" << motorBL.getCurrentSpeedNum() << ";" ";";
				debugFile << inputs[3] << ";" << motorBR.getCurrentSpeedNum() << "\n";
				/*
				std::cout << "MotorFL: " << motorFL.getCurrentSpeedNum() << std::endl;
				std::cout << "MotorFR: " << motorFR.getCurrentSpeedNum() << std::endl;
				std::cout << "MotorBL: " << motorBL.getCurrentSpeedNum() << std::endl;
				std::cout << "MotorBE: " << motorBR.getCurrentSpeedNum() << std::endl;
				*/

				//Loop sleep
				us now = getTimeU();
				us proccessTime = now - then;
				//std::cout << proccessTime.count() << std::endl;
				if (proccessTime < period)
				{
					//std::cout << "MotorLoop sleeping" << std::endl;
					std::this_thread::sleep_for(period - proccessTime);
				}
				then = getTimeU();
			}
			delay(10);
		}
	}
	catch (std::system_error error)
	{
		std::cout << error.code() << " meaning: " <<  error.what() << std::endl;
	}
	debugFile.close();
	std::cout << "exiting" << std::endl;
	display.writeStringToDisp("STOP");
	return;
}

void sensorReadTest()
{
	startTimer();
	wiringPiSetupGpio();
	// Mimics real operation
	IMUSensor sensor(SENSOR_BETA, true);
	us period{ (int)floor((1.0f / ANGLE_LOOP_FREQ * 1000000)) }; //Time for each loop itteration
	//std::cout << "Period: " << (int)period.count() << " us" << std::endl;
	auto then = getTimeU();
	us diff;
	us now;
	us proccessTime;
	us lastRead = getTimeU();
	//while (!newSensorMeasurement) {
	//	// Wait for first measurement
	//}
	
	while (true)
	{
		if (newSensorMeasurement)
		{
			us now = getTimeU();
			int readTime = (now - lastRead).count();
			std::cout << "readFreq		" << 1000000.0/(float)readTime << " Hz" << std::endl;
			lastRead = now;
			newSensorMeasurement = false;

			std::cout << "Roll:		" << sensor.roll.load() << " degrees" << std::endl;
			std::cout << "Pitch:	" << sensor.pitch.load() << " degrees" << std::endl;
			std::cout << "Yaw:		" << sensor.yaw.load() << " degrees" << std::endl;
			std::cout << "Reading Sensor " << std::endl;
			sensor.getAnglesMadgwick();			
		}
		else
		{
			sensor.MadgwickAHRSupdate();
		}

		delayMicroseconds(200);
		//now = getTimeU();
		//proccessTime = now - then;
		////std::cout << "proccessTime: " << (int)proccessTime.count() << std::endl;
		//if (proccessTime.count() < period.count())
		//{
		//	//std::cout << "sleepTime: " << (period - proccessTime).count() << " us" << std::endl;
		//	std::this_thread::sleep_for(period - proccessTime);
		//}
		////std::cout << "Time Slept: " << (getTimeU() - now).count() << " us" << std::endl;
		//then = getTimeU();
	}
}

void calibESCs()
{
	Motor motorFL(2);
	Motor motorFR(1);
	Motor motorBL(3);
	Motor motorBR(4);
	Motor_Driver driver(MOTOR_DRIVER_FREQ);
	Motor* motors[4] = { &motorFL, &motorFR, &motorBL, &motorBR };
	programmingMode(driver, motors);
}


int main(int argc, char *argv[])
{
	std::string name;
	if (argc == 1)
	{
		name = "Debug_Data";
	}
	else if (argc == 2)
	{
		name = "../../../DebugData/";
		name += argv[1];
		name += ".csv";
	}
	else
	{
		std::cout << "Too many args" << std::endl;
		return 1;
	}
	std::cout << "OutputFile: " << name << std::endl;;
	//testCopter(name);
	sensorReadTest();
	//calibESCs();
	std::cout << "Done" << std::endl;
	
	return 0;
}
