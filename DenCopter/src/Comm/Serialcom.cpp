#include "Serialcom.h"

SerialCom::SerialCom(char* deviceId, int baudrate)
{
	_fd = serialOpen(deviceId, baudrate);
}

SerialCom::~SerialCom()
{
	serialClose(_fd);
}
uint8_t SerialCom::readByte()
{
	while (serialDataAvail(_fd) < 1)
	{
		//std::cout << serialDataAvail(_fd) << std::endl;
	}
	//std::cout << "reading" << std::endl;
	int result;
	result = serialGetchar(_fd);
	return result;
}

int SerialCom::readBytes(uint8_t *buffer, int count)
{
	while (serialDataAvail(_fd) < count)
	{
		//std::cout << serialDataAvail(_fd) << std::endl;
	}
	if (read(_fd, buffer, count) != count)
	{
		std::cout << "Serial: Not enough data read" << std::endl;
	}
	return count;
}

void SerialCom::writeByte(uint8_t data)
{
	serialPutchar(_fd, data);
}

void SerialCom::writeBytes(uint8_t *buffer, int count)
{
	
	if (write(_fd, buffer, count) != count)
	{
		std::cout << "Serial: Not enough data written" << std::endl;
	}
		
}

void SerialCom::flushBuffer()
{
	serialFlush(_fd);
}

int SerialCom::readAvailableAmount()
{
	return serialDataAvail(_fd);
}