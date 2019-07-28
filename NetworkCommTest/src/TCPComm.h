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

//For multithreading
#include <thread>
#include <atomic>

#define PORT 8560

/*
	TEST:No Wireless logging class, is able to set up server, accept a request and respond with "REQUEST RECEIVED: X", with X the sequence number.
*/

/*
	Handles TCP Server, and spawns threads that handles the communication for the wireless logging.
*/
class TCPComm
{
public:
	// Constructor, accepts a Wireless Logging class
	TCPComm();

	~TCPComm();

	// Sets up the server
	void setupLoggingServer();

	// Starts the server, if available, creates new thread on connection and saves it in the list.
	void startLoggingServer();

	// Stops the server, if running
	void stopLoggingServer();
private:
	std::atomic_bool finishServer;
	
	// List with running threads. Each nonnegative element is a running thread. Max 10 loggerthreads in 1 session
	std::vector<std::thread> loggerThreads;


	// Filedesciptor of server
	int serverSocketFd;

	// Thread communication function
	void loggerThread(void* newsocketfd);

	// Function that fetches the last logger data
	void fetchLoggingData(float *data);

	// Function that writes the incoming PID data to the wireless logging and raises the event.
	void writePIDData();

	// Function that builds a string that can be sent over the socket.
	std::string buildMessageData(float* data, size_t length);
};