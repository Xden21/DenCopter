#ifndef _RX_INPUT_H_
#define _RX_INPUT_H_

#include <thread>
#include <atomic>
#include <unistd.h>
#include <cstdint>
#include <cmath>
#include <array>
#include "../Tools/Timer.h"
#include "Serialcom.h"

using namespace std::chrono_literals;

#define ARDUINO_ADDRESS "/dev/ttyACM0"
#define INPUT_FREQ 250 // So that the Serial port isn't overburdened.

class RXInput
{
public:
	RXInput() {};
	~RXInput() { stop_RX_Input(); };
	void start_RX_Input();
	void stop_RX_Input();
	void getInputs(float *inputs);
	bool getFailsafe();
	bool getAngleMode();
	bool getArmed();
	bool getShutdown();
	int getAltMode();
	bool getKillSwitch();
private:
	void rxInputThread();

	std::atomic<float> throttle{ 0 };
	std::atomic<float> roll{ 0 };
	std::atomic<float> pitch{ 0 };
	std::atomic<float> yaw{ 0 };
	std::atomic<int> altMode{ 0 };
	std::atomic<bool> armed{ false };
	std::atomic<bool> failsafe{ false };
	std::atomic<bool> decoding{ true };
	std::atomic<bool> angleMode{ true };
	std::atomic<bool> shutdown{ false };
	std::atomic<bool> killswitch{ false };
	std::thread RXInputThread;

	//long counter = 0;
};


#endif // !_RX_INPUT_H_
