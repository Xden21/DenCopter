#include "WirelessLogging.h"

WirelessLogging::WirelessLogging(Controller* flightController)
{
	this->flightController = flightController;
	server.setupLoggingServer();
	Attitude_PIDData pidData;
	if (flightController->getAngleMode())
	{
		pidData.yaw_KP = flightController->yaw_angle_KP;
		pidData.yaw_KI = flightController->yaw_angle_KI;
		pidData.yaw_KD = flightController->yaw_angle_KD;
		pidData.pitch_KP = flightController->pitch_angle_KP;
		pidData.pitch_KI = flightController->pitch_angle_KI;
		pidData.pitch_KD = flightController->pitch_angle_KD;
		pidData.roll_KP = flightController->roll_angle_KP;
		pidData.roll_KI = flightController->roll_angle_KI;
		pidData.roll_KD = flightController->roll_angle_KD;
	}
	else
	{
		pidData.yaw_KP = flightController->yaw_rate_KP;
		pidData.yaw_KI = flightController->yaw_rate_KI;
		pidData.yaw_KD = flightController->yaw_rate_KD;
		pidData.pitch_KP = flightController->pitch_rate_KP;
		pidData.pitch_KI = flightController->pitch_rate_KI;
		pidData.pitch_KD = flightController->pitch_rate_KD;
		pidData.roll_KP = flightController->roll_rate_KP;
		pidData.roll_KI = flightController->roll_rate_KI;
		pidData.roll_KD = flightController->roll_rate_KD;
	}
	server.setAttitudePIDData(&pidData);
	finished = false;
}

WirelessLogging::~WirelessLogging()
{
	stopLogging();
}

void WirelessLogging::startLogging(float* initialLoggingData)
{
	PIDDataThread = std::thread(&WirelessLogging::PIDDataThreadFunc, this);
	server.setLoggingData(initialLoggingData, LOGCOUNT);
	server.startLoggingServer(); // Starts logging server thread
}

void WirelessLogging::stopLogging()
{
	finished = true;
	server.stopLoggingServer();
}

void WirelessLogging::setLoggingData(float* data)
{
	server.setLoggingData(data, LOGCOUNT);
}

void WirelessLogging::PIDDataThreadFunc()
{
	while (!finished)
	{
		std::unique_lock<std::mutex> PIDLock(server.PIDMutex);
		server.PIDSignal.wait(PIDLock, [&] {return server.PIDDataAvailable; });
		server.PIDDataAvailable = false;
		Attitude_PIDData data;
		server.getAttitudePIDData(&data);
		PIDLock.unlock();
		std::unique_lock<std::mutex> PIDContrlLock(flightController->PIDMutex);
		if (flightController->getAngleMode())
		{
			flightController->updateAnglePids(&data);
		}
		else
		{
			flightController->updateRatePids(&data);
		}
	}
}