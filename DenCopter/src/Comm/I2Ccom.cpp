#include "I2Ccom.h"

int readBlind(int fd, uint8_t *buffer, int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	if (read(fd, buffer, count) != count)
	{
		std::cout << "I2C: Not enough data read, must be: " << count << std::endl;
		return -1;
	}
	return count;	
}

uint8_t readReg8(int fd, uint8_t subaddress)
{
	std::lock_guard<std::mutex> lock(iomutex);
	uint8_t data = wiringPiI2CReadReg8(fd, subaddress);
	return data;
	
}

uint16_t readReg16(int fd, uint8_t subaddress)
{
	std::lock_guard<std::mutex> lock(iomutex);
	uint16_t data = wiringPiI2CReadReg16(fd, subaddress);
	return data;

}

int readRegMult(int fd, uint8_t start_subaddress, uint8_t *buffer,int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	for (int i = 0; i < count; i++)
	{
		buffer[i] = wiringPiI2CReadReg8(fd, start_subaddress + i);
	}
	return count;
}

int readRegList(int fd, uint8_t start_subaddress, uint8_t *buffer, int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	if (count <= 32)
	{
		return (i2c_smbus_read_i2c_block_data(fd, start_subaddress, count, buffer));
	}
	else
	{
		uint8_t reg[1]{ start_subaddress };
		if (write(fd, reg, 1) != 1)
		{
			std::cout << "I2C: bad long read" << std::endl;
			return -1;
		}
		if (read(fd, buffer, count) != count)
		{
			std::cout << "I2C: bad long read" << std::endl;
			return -1;
		}
		return count;
	}
}

int writeRegList(int fd, uint8_t start_subaddress, uint8_t *buffer, int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	return (i2c_smbus_write_i2c_block_data(fd, start_subaddress, count, buffer));
}

int writeBlind(int fd, uint8_t *buffer, int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	if (write(fd, buffer, count) != count){
	
		std::cout << "I2C: Not enough data written" << std::endl;
		return -1;
	}
	return count;
}

int write8(int fd, uint8_t data)
{
	std::lock_guard<std::mutex> lock(iomutex);
	int result = wiringPiI2CWrite(fd,  data);
	return result;
}

void writeReg8(int fd, uint8_t subaddress, uint8_t data)
{
	std::lock_guard<std::mutex> lock(iomutex);
	wiringPiI2CWriteReg8(fd, subaddress, data);
	return;
}

void writeRegMult(int fd, uint8_t start_subaddress, uint8_t *data, int count)
{
	std::lock_guard<std::mutex> lock(iomutex);
	for (int i = 0; i < count; i++)
	{
		wiringPiI2CWriteReg8(fd, start_subaddress + i, data[i]);
	}
	return;
}