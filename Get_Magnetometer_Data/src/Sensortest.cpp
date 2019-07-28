#include "IMUSensor.h"
#include "Timer.h"
#include <iostream>
#include <thread>

void sensorTest()
{
	startTimer();
	IMUSensor sensor(3);
	us period{ (int)floor((1.0f / 4000 * 1000000)) };
	us then = getTimeU();
	int count = 0;
	sensor.getAnglesMadgwick();
	while (true)
	{
		sensor.MadgwickAHRSupdate();
		if (count >= 8)
		{
			sensor.getAnglesMadgwick();
			count = 0;
			std::cout << "Roll: " << sensor.roll.load() << std::endl;
			std::cout << "Pitch: " << sensor.pitch.load() << std::endl;
			std::cout << "Yaw: " << sensor.yaw.load() << std::endl;
		}
		else
		{
			count++;
		}
		us now = getTimeU();
		us proccessTime = now - then;
		if (proccessTime < period)
		{
			std::this_thread::sleep_for(period - proccessTime);
		}
		then = getTimeU();
	}
}

int main()
{
	sensorTest();
	return 0;
}
