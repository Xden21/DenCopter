#pragma once
#include <wiringPi.h>
#include <wiringSerial.h>
#include <cstdint>
#include <iostream>
#include <unistd.h>

#define BAUDRATE 57600

class SerialCom
{
public:
	SerialCom(char* deviceId, int baudrate);
	~SerialCom();
	/*
	Reads a single byte over serial if available, waits until data is received.
	*/
	uint8_t readByte();

	/*
	Reads count bytes over serial, waits until data is available.
	*/
	int readBytes(uint8_t *buffer, int count);

	/*
	Writes a single byte over serial to destination.
	*/
	void writeByte(uint8_t data);

	/*
	Writes a series of bytes over serial to destination.
	*/
	void writeBytes(uint8_t *buffer, int count);

	void flushBuffer();

	int readAvailableAmount();
private:
	int _fd;
};
