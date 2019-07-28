/*



DEPRICATED!!



*/		

#include "PCInput.h"

PCInput::PCInput()
{
}

PCInput::~PCInput()
{
	PCInputThread.join();
}

void PCInput::getInputs(float *inputs)
{
	inputs[0] = PC_rollInput.load();
	inputs[1] = PC_pitchInput.load();
	inputs[2] = PC_yawInput.load();
	inputs[3] = 50;				//Throttle set at 50 for the moment
}

//Choose angle to control an give in desired setpoint.
void PCInput::pcInputThread()
{
	bool done = false;
	int inputValue;
	while (!done)
	{
		std::string mode;
		std::cout << "Enter mode: angle or rate" << std::endl;
		std::cin >> mode;
		if (mode == "angle")
		{
			angle_mode.store(true);
			input_init_done.store(true);
		}
		else if (mode == "rate")
		{
			angle_mode.store(false);
			input_init_done.store(true);
		}
		else
		{
			continue;
		}

		std::string angle;
		std::cout << "Give angle: roll, pitch or yaw" << std::endl;
		std::cin >> angle;
		if (angle == "roll")
		{
			if (angle_mode.load())
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 30 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue >= 0)
					{
						PC_rollInput.store(inputValue);
					}
					else
					{
						PC_rollInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}
			else
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 100 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue > 0)
					{
						PC_rollInput.store(inputValue);
					}
					else
					{
						PC_rollInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}
		}
		else if (angle == "pitch")
		{
			if (angle_mode.load())
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 30 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue > 0)
					{
						PC_pitchInput.store(inputValue);
					}
					else
					{
						PC_pitchInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}
			else
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 100 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue > 0)
					{
						PC_pitchInput.store(inputValue);
					}
					else
					{
						PC_pitchInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}

		}
		else if (angle == "yaw")
		{
			if (angle_mode.load())
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 30 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue > 0)
					{
						PC_yawInput.store(inputValue);
					}
					else
					{
						PC_yawInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}
			else
			{
				while (!done)
				{
					std::cout << "Give value between 0 and 100 or -1 to stop" << std::endl;
					std::cin >> inputValue;
					if (inputValue > 0)
					{
						PC_yawInput.store(inputValue);
					}
					else
					{
						PC_yawInput.store(0);
						stop.store(true);
						done = true;
					}
				}
			}

		}
	}
}

void PCInput::startPcInputThread()
{
	PCInputThread = std::thread(&PCInput::pcInputThread, this);
}