#pragma once
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <thread>
#include <mutex>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <linux/i2c-dev.h>
/**
	A set of helper functions to communicate over I2C with wiringpi.
	these make sure the communication is done thread safe.
*/

/*
	The mutex used to lock the access to the functions
*/
static std::mutex iomutex;

/*
	Read the tx buffer from the given address.
*/
int readBlind(int fd, uint8_t *buffer, int count);

/*
	Read the given subaddress/register from the given address.
*/
uint8_t readReg8(int fd, uint8_t subaddress);

uint16_t readReg16(int fd, uint8_t subaddress);

/*
	Read the given subaddresses/registers from the given address.
	Starts at given subaddress and reads the (count-1) subsequent ones.
*/
int readRegMult(int fd, uint8_t start_subaddress, uint8_t *buffer,  int count);

/*
	Read data in a block.
*/
int readRegList(int fd, uint8_t start_subaddress, uint8_t *buffer, int count);

/*
	Write to the rx buffer of the given address.
*/
int writeBlind(int fd, uint8_t *buffer, int count);

/*
	Write to the given subaddress/register of the given address.
*/
void writeReg8(int fd, uint8_t subaddress, uint8_t data);

/*
	Write data to device with given address.
*/
int write8(int fd, uint8_t data);

/*
	Write data in consecutive writes.
*/
void writeRegMult(int fd, uint8_t start_subaddress, uint8_t *data, int count);

/*
	Write data in a block.
*/
int writeRegList(int fd, uint8_t start_subaddress, uint8_t *buffer, int count);
