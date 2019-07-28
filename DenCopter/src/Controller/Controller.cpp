#include "Controller.h"

//Conf file name
const std::string Controller::confFileName = "PIDTuning.conf";

////////////////////////////////////////////////////////////////
// Constructor and destructor
// Sets all neccesairy objects needed to control the quadcopter.
// Vars: Motor_Driver, IMUSensor, Motors

Controller::Controller(Motor_Driver* driver_object, IMUSensor* sensor_object, BMP280* altitude_sensor_object, Motor** motors)
	:driver(driver_object), sensor(sensor_object), altitude_sensor(altitude_sensor_object)
{
	motorFL = motors[0];
	motorFR = motors[1];
	motorBL = motors[2];
	motorBR = motors[3];

	altitude_filter = new Altitude_KF(0.3, 0.2);

	confFile = new ConfigHandler(confFileName);

	//Conf handling
	if (confFile->exists())
	{
		readConfig();
		std::cout << "Configuration succesfully loaded in." << std::endl;
	}
	else
	{
		writeConfig();
		exitWithError("ConfigFile written, fill in and restart.");
	}
}

// Finishes the threads to clean up.
Controller::~Controller()
{
	if (!landed.load())
	{
		finishThreads();
	}
}

void Controller::writeConfig()
{
	std::vector<std::string> keys{ "#PID Tuning parameters",";",
									"throttle_var", "lift_point",";",
									"yaw_rate_KP","yaw_rate_KD","yaw_rate_KI",";",
									"pitch_rate_KP", "pitch_rate_KD", "pitch_rate_KI",";",
									"roll_rate_KP", "roll_rate_KD", "roll_rate_KI",";",
									"max_yaw_rate", "max_pitch_rate", "max_roll_rate",";",
									"max_yaw_angle", "max_pitch_angle", "max_roll_angle", ";",
									"pitch_angle_KP", "pitch_angle_KD", "pitch_angle_KI",";",
									"roll_angle_KP", "roll_angle_KD", "roll_angle_KI",";",
									"yaw_angle_KP", "yaw_angle_KD", "yaw_angle_KI" }; //TODO Add height control vars.
	confFile->write(keys);
}

void Controller::readConfig()
{
	confFile->read();

	//Throttle conf
	throttle_var = confFile->getValueOfKey<float>("throttle_var");
	lift_point = confFile->getValueOfKey<float>("lift_point");

	//Rate PIDS
	yaw_rate_KP = confFile->getValueOfKey<float>("yaw_rate_KP");
	yaw_rate_KD = confFile->getValueOfKey<float>("yaw_rate_KD");
	yaw_rate_KI = confFile->getValueOfKey<float>("yaw_rate_KI");

	pitch_rate_KP = confFile->getValueOfKey<float>("pitch_rate_KP");
	pitch_rate_KD = confFile->getValueOfKey<float>("pitch_rate_KD");
	pitch_rate_KI = confFile->getValueOfKey<float>("pitch_rate_KI");

	roll_rate_KP = confFile->getValueOfKey<float>("roll_rate_KP");
	roll_rate_KD = confFile->getValueOfKey<float>("roll_rate_KD");
	roll_rate_KI = confFile->getValueOfKey<float>("roll_rate_KI");

	max_yaw_rate = confFile->getValueOfKey<float>("max_yaw_rate");
	max_pitch_rate = confFile->getValueOfKey<float>("max_pitch_rate");
	max_roll_rate = confFile->getValueOfKey<float>("max_roll_rate");

	//Angle PIDS

	max_yaw_angle = confFile->getValueOfKey<float>("max_yaw_angle");
	max_pitch_angle = confFile->getValueOfKey<float>("max_pitch_angle");
	max_roll_angle = confFile->getValueOfKey<float>("max_roll_angle");

	pitch_angle_KP = confFile->getValueOfKey<float>("pitch_angle_KP");
	pitch_angle_KD = confFile->getValueOfKey<float>("pitch_angle_KD");
	pitch_angle_KI = confFile->getValueOfKey<float>("pitch_angle_KI");

	roll_angle_KP = confFile->getValueOfKey<float>("roll_angle_KP");
	roll_angle_KD = confFile->getValueOfKey<float>("roll_angle_KD");
	roll_angle_KI = confFile->getValueOfKey<float>("roll_angle_KI");

	yaw_angle_KP = confFile->getValueOfKey<float>("yaw_angle_KP");
	yaw_angle_KD = confFile->getValueOfKey<float>("yaw_angle_KD");
	yaw_angle_KI = confFile->getValueOfKey<float>("yaw_angle_KI");
}

////////////////////////////////////////////////////////////////
// Init function
// Initialises the PID controllers.
void Controller::initController()
{
	int kplist[PID_AMOUNT] = { 1,1,1,1,1,1,1,1 };
	int kilist[PID_AMOUNT] = { 0,0,0,0,0,0,0,0 };
	int kdlist[PID_AMOUNT] = { 0,0,0,0,0,0,0,0 };

	//Init the controllersr with basic gain values, to be adjusted later on init of the quadcopter itself.
	for (int i = 0; i < PID_AMOUNT; i++)
	{
		pids[i].initPID(kplist[i], kilist[i], kdlist[i], 0, 0);
	}
}

// Sets the landed state which starts or stops the threads neccesairy to fly dependant on the selected fly mode.
void Controller::setLanded(bool landed_arg)
{
	if (landed_arg)
	{
		landed.store(landed_arg); //This will stop the pid threads
		motorSpeeds[0] = 0;
		motorSpeeds[1] = 0;
		motorSpeeds[2] = 0;
		motorSpeeds[3] = 0;
		finishThreads();
		std::cout << "Setting Landed" << std::endl;
	}
	else if (!landed_arg && landed.load())
	{
		std::cout << "Setting Up Thread" << std::endl;
		landed.store(landed_arg);		//If threads not running, start them
		if (angleMode)
		{
			setupAnglePids();
			startAngleThread();
		}
		else
		{
			setupRatePids();
			startRateThread();
		}
	}
}

// Returns the landed state.
bool Controller::getLanded()
{
	return landed.load();
}

// Arms the motors, so they respond to speed commands.
void Controller::armMotors()
{
	driver->armMotor(*motorFL);
	driver->armMotor(*motorFR);
	driver->armMotor(*motorBL);
	driver->armMotor(*motorBR);
	std::cout << "Armed" << std::endl;
}

// Disarms the motots, stopping them. They won't respond to speed commands.
void Controller::disarmMotors()
{
	driver->disarmMotor(*motorFL);
	driver->disarmMotor(*motorFR);
	driver->disarmMotor(*motorBL);
	driver->disarmMotor(*motorBR);
	sleep(5);
}

// Calculate the speeds for the motors (see getMotorSpeeds) and set the motors.
// Motors must be armed to work!
void Controller::updateMotors()
{
	//Won't do a thing when motors are set to disarm!
	getMotorSpeeds();
	if (!killSwitch)
	{
		driver->setMotor(*motorFL, motorSpeeds[MOTORFL]);
		driver->setMotor(*motorFR, motorSpeeds[MOTORFR]);
		driver->setMotor(*motorBL, motorSpeeds[MOTORBL]);
		driver->setMotor(*motorBR, motorSpeeds[MOTORBR]);
	}
	else
	{
		driver->setMotor(*motorFL, 0);
		driver->setMotor(*motorFR, 0);
		driver->setMotor(*motorBL, 0);
		driver->setMotor(*motorBR, 0);
	}
}

// Set the control mode to angle or rate mode control.
// If true: Then inputs will be handled as angles, else: inputs will be handled as turn rates.
void Controller::setAngleMode(bool angleMode_arg)
{
	angleMode = angleMode_arg;
}

// Returns the mode thats used to interpret the inputs.
bool Controller::getAngleMode()
{
	return angleMode;
}

//Warms up sensor to get good readings at flight start
void Controller::sensorWarmUp()
{
	std::cout << "Sensor Warmup" << std::endl;
	sensor->sensorWarmUp();
}

bool Controller::useBaro()
{
	return (altHold.load() || altStabilize.load());
}

void Controller::setInput(float* inputs, int altMode, bool killSwitch) 
{
	setSetpoints(inputs, altMode); //Set setpoints for the controller
	this->killSwitch.store(killSwitch);
}

void Controller::getAttitudeData(float* data)
{
	data[0] = sensor->yaw;
	data[1] = sensor->pitch;
	data[2] = sensor->roll;
}