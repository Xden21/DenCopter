#pragma once
#include "../Comm/TCPComm.h"
#include "../Controller/Controller.h"

#define LOGCOUNT 6 // y,p,r desired and current

class WirelessLogging
{
public:
	// Constructor
	WirelessLogging(Controller* flightController);

	~WirelessLogging();

	// Starts the server and PID thread
	void startLogging(float* initialLoggingData);

	void stopLogging();

	// Sets the logging data
	void setLoggingData(float* data);

	private:
	// The server
	TCPComm server;

	// Thread that updates the PID data when it comes in.
	std::thread PIDDataThread;

	// The flight controller, used to update PID tuning.
	Controller* flightController;

	// Handles the PID data update.
	void PIDDataThreadFunc();

	std::atomic_bool finished;
};