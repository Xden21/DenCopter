#include "Timer.h"

// Timer functions in microseconds.

void startTimer()
{
	if (!timer_started)
	{
		startTime = chrono::now();
		timer_started = true;
	}
}

us getTimeU()
{
	if (timer_started == true)
	{
		auto now = chrono::now();
		auto delta = now - startTime;
		auto usecTime = std::chrono::duration_cast<us>(delta);
		return usecTime;
	}
	else
	{
		std::cout << "Timer not started, return is equal to 0, please start timer using 'startTimer()'" << std::endl;
		return (us) 0;
	}

}