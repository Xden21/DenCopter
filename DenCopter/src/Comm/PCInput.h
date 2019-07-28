
#ifndef _PC_INPUT_H_
#define _PC_INPUT_H_

#include <thread>
#include <iostream>
#include <string>
#include <atomic>

class PCInput
{
public:
	PCInput();
	~PCInput();
	void getInputs(float *inputs);
	void startPcInputThread();

	std::atomic<bool> input_init_done{ false };
	std::atomic<bool> angle_mode{ false };
	std::atomic<bool> stop{ false };
private:
	void pcInputThread();

	std::thread PCInputThread;
	std::atomic<float> PC_yawInput{ 0 };
	std::atomic<float> PC_pitchInput{ 0 };
	std::atomic<float> PC_rollInput{ 0 };
};

#endif