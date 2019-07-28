#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "../Domain/PIDData.h"

//For multithreading
#include <thread>
#include <atomic>

#define PORT 8560
#define MAX_CLIENTS 5
#define PID_DATA_COUNT 9

/*
	TEST:No Wireless logging class, is able to set up server, accept a request and respond with "REQUEST RECEIVED: X", with X the sequence number.
*/

/*
	Handles TCP Server, and spawns threads that handles the communication for the wireless logging.
*/
class TCPComm
{
public:
	std::mutex PIDMutex;
	std::mutex LogDataMutex;

	std::condition_variable PIDSignal;
	
	bool PIDDataAvailable;

	// Constructor
	TCPComm();

	~TCPComm();

	// Sets up the server
	void setupLoggingServer();

	// Starts the server, if available, creates new thread on connection and saves it in the list.
	void startLoggingServer();

	// Stops the server, if running
	void stopLoggingServer();

	// Function that sets logger data to send
	void setLoggingData(float* data, size_t amount);

	// Function that fetches the incoming PID data to the wireless logging.
	void getAttitudePIDData(Attitude_PIDData* data);

	void setAttitudePIDData(Attitude_PIDData* data);

	// Todo altitude

private:
	std::atomic_bool finishServer;
	
	std::atomic_bool newPIDData;

	float* loggingData;

	float PIDData_Raw[PID_DATA_COUNT];

	void loggingServer();

	size_t logAmount;

	// List with running threads. Each nonnegative element is a running thread. Max 10 loggerthreads in 1 session
	std::vector<std::thread> loggerThreads;

	std::thread loggingServerThread;

	int clientAmount;

	// Filedesciptor of server
	int serverSocketFd;

	// Thread communication function
	void loggerThread(void* newsocketfd);

	// Function that builds a string that can be sent over the socket.
	std::string buildMessageData(float* data, size_t length);
};