
#include "ESC_Programming_Mode.h"

// Method for programming the esc for the motors.
void programmingMode(Motor_Driver &driver, Motor** motors)
{
	driver.stopMotor(*motors[0]);
	driver.stopMotor(*motors[1]);
	driver.stopMotor(*motors[2]);
	driver.stopMotor(*motors[3]);

	std::cout << "To Start Calib press Enter" << std::endl;
	std::cin.get();
	driver.setMotor(*motors[0], 100, true);
	driver.setMotor(*motors[1], 100, true);
	driver.setMotor(*motors[2], 100, true);
	driver.setMotor(*motors[3], 100, true);

	std::cout << "Connect battery now and press enter... " << std::endl;
	std::cin.get();
	std::cout << "There will be a beep in 2 seconds" << std::endl;
	sleep(2);
	driver.setMotor(*motors[0], 0, true);
	driver.setMotor(*motors[1], 0, true);
	driver.setMotor(*motors[2], 0, true);
	driver.setMotor(*motors[3], 0, true);

	std::cout << "ESC is entering work mode now, unplug battery pack" << std::endl;
	sleep(5);
	return;
}
