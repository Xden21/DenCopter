#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <unistd.h>
#include "Serialcom.h"
#include "Timer.h"
#include <cmath>
#include <thread>


char* arduino_address = "/dev/ttyACM0";
int baudrate = 9600;

uint8_t buffer[25];
int armed_l;
int shutdown_l;
int throttle_l;
int roll_l;
int pitch_l;
int yaw_l;
int failsafe_l;
int altMode_l;
int main(void)
{
	startTimer();
	SerialCom com(arduino_address, baudrate); //Start communication.
	us period{ (int)floor((1.0f / 100 * 1000000)) }; //period time.
	us then = getTimeU(); //Start time.
	for (int i = 0; i < 1000; i++)
	{
		com.writeByte(1); //Request data.
		com.readBytes(buffer, 25);
		std::cout << (int)((buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF) << std::endl; //Print throttle.
		std::cout << (int)((buffer[12] | buffer[13] << 8) & 0x07FF) << std::endl;//Print altmode
		com.writeByte(1); //Data aknowledgde
		delay(2);
		roll_l = (buffer[1] | buffer[2] << 8) & 0x07FF;									//Channel 1
		pitch_l = (buffer[2] >> 3 | buffer[3] << 5) & 0x07FF;								//Channel 2
		throttle_l = (buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF;	//Channel 3
		yaw_l = (buffer[5] >> 1 | buffer[6] << 7) & 0x07FF;								//Channel 4
		armed_l = (buffer[6] >> 4 | buffer[7] << 4) & 0x07FF;		//Channel 5
		shutdown_l = (buffer[10] >> 5 | buffer[11] << 3) & 0x07FF;	//Channel 8
		failsafe_l = (buffer[23] >> 3) & 0x0001; //Failsafe
		altMode_l = ((buffer[12] | buffer[13] << 8) & 0x07FF); //Channel 9

		uint8_t sendbuffer[15];
		sendbuffer[0] = (armed_l & 0x0FF);
		sendbuffer[1] = (armed_l & 0x0700) >> 8;
		sendbuffer[2] = (shutdown_l & 0x0FF);
		sendbuffer[3] = (shutdown_l & 0x0700) >> 8;
		sendbuffer[4] = (throttle_l & 0x0FF);
		sendbuffer[5] = (throttle_l & 0x0700) >> 8;
		sendbuffer[6] = (roll_l & 0x0FF);
		sendbuffer[7] = (roll_l & 0x0700) >> 8;
		sendbuffer[8] = (pitch_l & 0x0FF);
		sendbuffer[9] = (pitch_l & 0x0700) >> 8;
		sendbuffer[10] = (yaw_l & 0x0FF);
		sendbuffer[11] = (yaw_l & 0x0700) >> 8;
		sendbuffer[12] = failsafe_l;
		sendbuffer[13] = (altMode_l & 0x0FF);
		sendbuffer[14] = (altMode_l & 0x0700) >> 8;

		com.writeBytes(sendbuffer, 15);
		delay(2);
		std::cout << "Data Ok: " << (int)com.readByte() << std::endl;

		auto now = getTimeU();
		auto proccessTime = now - then;
		if (proccessTime < period)
		{
			std::cout << "RX thread sleeping" << std::endl;
			std::this_thread::sleep_for(period - proccessTime);
		}
		then = getTimeU();
	}
}

