#include "Motor_Driver.h"

///////////////////////////////////////////////////////////////////////
//Constructor and destructor

// Initialise the driver board and set all outputs to 0.
Motor_Driver::Motor_Driver(int freq) : _freq(freq)
{
	if (wiringPiSetupGpio() == -1)
		return;
	fileDescriptorMOT = wiringPiI2CSetup(MOTOR_DRIVER_ADDRESS);
	init();
	begin();
	setPWM_freq(freq);
	writeReg8(fileDescriptorMOT, ALL_LED_ON_L, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_H, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_L, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_H, 0);

	_init = true;
}

// Set all outputs to 0 and order a orderly shutdown to the board.
Motor_Driver::~Motor_Driver()
{
	if (!_init)
	{
		return;
	}
	writeReg8(fileDescriptorMOT, ALL_LED_ON_L, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_H, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_L, 0);
	writeReg8(fileDescriptorMOT, ALL_LED_OFF_H, 0x10);
	uint8_t config = readReg8(fileDescriptorMOT, MODE1);
	config = (config & 0x0F) | (1 << 4);
	writeReg8(fileDescriptorMOT, MODE1, config);				//Set sleep (orderly shutdown)
}

/////////////////////////////////////////////////////////////////////////
//Public Functions

// Settings for the board.
void Motor_Driver::init()
{
	settings.mode1.restart = false;
	settings.mode1.auto_increment_enable = true;
	settings.mode1.sleep_enable = false;
	settings.mode1.sub1_enable = false;
	settings.mode1.sub2_enable = false;
	settings.mode1.sub3_enable = false;
	settings.mode1.allcall_enable = true;
}

// Apply settings to the board.
void Motor_Driver::begin()
{
	uint8_t tempreg = 0;
	int x = settings.mode1.auto_increment_enable ? (1 << 5) : 0;
	tempreg |= x;

	x = 0;
	x = settings.mode1.sub1_enable ? (1 << 3) : 0;
	tempreg |= x;
	
	x = 0;
	x = settings.mode1.sub2_enable ? (1 << 2) : 0;
	tempreg |= x;

	x = 0;
	x = settings.mode1.sub3_enable ? (1 << 1) : 0;
	tempreg |= x;

	x = 0;
	x = settings.mode1.allcall_enable ? 1 : 0;
	tempreg |= x;

	writeReg8(fileDescriptorMOT, MODE1, tempreg);
	writeReg8(fileDescriptorMOT, MODE2, OUTDRV);
	usleep(500);
	uint8_t mode1 = readReg8(fileDescriptorMOT, MODE1);
	mode1 = mode1 & ~(1 << 4); //Disable sleep
	writeReg8(fileDescriptorMOT, MODE1, mode1);
	usleep(500);

	_init = true;
	std::cout << "driver initialised" << std::endl;
}

// Set a motor to a desired speed.
// Will only work if the motor is armed.
// Overide possible (ONLY FOR PROGRAMMING THE ESC!!!)
void Motor_Driver::setMotor(Motor &motor, float speed, bool armoveride)
{
	if (!_init)
	{
		return;
	}
	
	else if (!motor.getArmStatus() & !armoveride)
	{
		return;
	}
	
	if (speed > 100)
		speed = 100;
	else if (speed < 0)
		speed = 0;

	motor.setCurrentSpeedNum(speed);
	//std::cout << "set motor" << motor.getMotorID()+1 << "to " << speed << "%" << std::endl;
	speed /= 100;
	float duty = (DUTY_MIN + (DUTY_MAX - DUTY_MIN) * speed)/100.0f;
	uint16_t tick_count = (uint16_t)(duty * TICK_RANGE) - 1;
	//int tick_count = static_cast<int>(_ms_to_tick + speed*_ms_to_tick);
	//std::cout << "Setting ticks to " << tick_count<< std::endl;

	motor.setCurrentSpeedPWM(tick_count);
	uint8_t tick_count_L = tick_count & 0xFF;
	uint8_t tick_count_H = (tick_count & 0xF00) >> 8;
	uint8_t buffer[4] = { 0,0,tick_count_L,tick_count_H };
	writeRegMult(fileDescriptorMOT, LED0_ON_L + 4 * motor.getMotorID(), buffer, 4);
	/*
	writeByte(LED0_OFF_L + 4 * motor.getMotorID(), tick_count_L);
	writeByte(LED0_OFF_H + 4 * motor.getMotorID(), tick_count_H);
	writeByte(LED0_ON_L + 4 * motor.getMotorID(), 0);
	writeByte(LED0_ON_H + 4 * motor.getMotorID(), 0);	
	*/
}

// Arm a motor.
void Motor_Driver::armMotor(Motor &motor)
{
	motor.setArmed(true);
	setMotor(motor, 0);
}

// Disarm a motor.
void Motor_Driver::disarmMotor(Motor &motor)
{
	stopMotor(motor);
	motor.setArmed(false);
}

// Stop a motor.
void Motor_Driver::stopMotor(Motor &motor)
{
	uint8_t buffer[4] = { 0,0,0,0 };
	writeRegMult(fileDescriptorMOT, LED0_ON_L + 4 * motor.getMotorID(), buffer, 4);
	/*
	writeByte(LED0_OFF_L + 4 * motor.getMotorID(), 0);
	writeByte(LED0_OFF_H + 4 * motor.getMotorID(), 0);
	writeByte(LED0_ON_L + 4 * motor.getMotorID(), 0);
	writeByte(LED0_ON_H + 4 * motor.getMotorID(), 0);
	*/
}

// Set PWM frequency of the driver board output.
void Motor_Driver::setPWM_freq(int freq)
{
	std::cout << "Setting frequency to " << freq << " hertz" << std::endl;
	if (!_init)
	{
		return;
	}
	int prescale_value = round(25000000 / ((float)freq * 4096)) - 1;
	std::cout << "Setting prescale value to " << prescale_value << std::endl;
	uint8_t old_reg = readReg8(fileDescriptorMOT, MODE1);
	uint8_t new_reg = (old_reg & 0x7F) | (1 << 4);
	writeReg8(fileDescriptorMOT, MODE1, new_reg);
	writeReg8(fileDescriptorMOT, PRE_SCALE, prescale_value);
	writeReg8(fileDescriptorMOT, MODE1, old_reg);
	usleep(500);
	writeReg8(fileDescriptorMOT, MODE1, old_reg | (1 << 7));
	_freq = freq;
}


//////////////////////////////////////////////////////////////////////////
//Private Functions

//////////////////////////////////////////////////////////////////////////
// I2C Communication functions.

