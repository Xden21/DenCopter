#include "RXInput.h"

// Thread to read the input from the Arduino (including double check).
void RXInput::rxInputThread()
{
	//std::cout << "In input thread" << std::endl;

	//Double control variables
	bool shutdown_control = false;
	bool arm_control = false;
	int armed_l;
	int shutdown_l;
	int throttle_l;
	int roll_l;
	int pitch_l;
	int yaw_l;
	int failsafe_l;
	int altMode_l;
	int angleMode_l;
	int decoding_l;
	int checksum_l;
	int killswitch_l;
	int lastArmed;
	int lastShutdown;
	int lastThrottle;
	int lastRoll;
	int lastPitch;
	int lastYaw;
	int lastFailsafe;
	int lastAltMode;
	int lastAngleMode;
	int lastDecoding;
	int lastkillswitch;
	int checksum;

	uint8_t control_ok;
	SerialCom com(ARDUINO_ADDRESS, BAUDRATE);
	us period{ (int)floor(((1.0f / INPUT_FREQ) * 1000000)) };
	uint8_t buffer[26]{ 0 };

	us then = getTimeU();
	us now;
	us proccessTime;

	com.flushBuffer();
	while (!getShutdown())
	{
		auto normalize = [](int value)
		{
			float normvalue = ((value / 16) - 12);
			normvalue = (normvalue < 0) ? 0 : normvalue;
			normvalue = (normvalue > 100) ? 100 : normvalue;
			return normvalue;
		};
		auto normalizethrot = [](int value)
		{
			float normvalue((value / 16) - 11.875);
			normvalue = (normvalue < 0) ? 0 : normvalue;
			normvalue = (normvalue > 100) ? 100 : normvalue;
			return normvalue;
		};
		com.writeByte(1); //Write request byte
		if(com.readBytes(buffer, 27) != 27) // If not enough data is read, the communication is faulty and killswitch should be activated as safety measure
		{
			killswitch.store(true);
		}
		else
		{
			com.writeByte(1); //Write aknowledge byte
			delay(1);
			roll_l = (buffer[1] | buffer[2] << 8) & 0x07FF;									//Channel 1
			pitch_l = (buffer[2] >> 3 | buffer[3] << 5) & 0x07FF;								//Channel 2
			throttle_l = (buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF;	//Channel 3
			yaw_l = (buffer[5] >> 1 | buffer[6] << 7) & 0x07FF;								//Channel 4
			armed_l = (buffer[6] >> 4 | buffer[7] << 4) & 0x07FF;		//Channel 5
			angleMode_l = (buffer[7] >> 7 | buffer[8] << 1 | buffer[9] << 9) & 0x07FF;
			decoding_l = (buffer[9] >> 2 | buffer[10] << 6) & 0x07FF;
			//angleMode.store((normalize((buffer[7] >> 7 | buffer[8] << 1 | buffer[9] << 9) & 0x07FF) > 50) ? true : false); //Channel 6
			//decoding.store(((normalize((buffer[9] >> 2 | buffer[10] << 6) & 0x07FF) > 50) ? true : false));	//Channel 7
			shutdown_l = (buffer[10] >> 5 | buffer[11] << 3) & 0x07FF;	//Channel 8
			failsafe_l = (buffer[23] >> 3) & 0x0001; //Failsafe
			altMode_l = ((buffer[12] | buffer[13] << 8) & 0x07FF); //Channel 9
			killswitch_l = ((buffer[13] >> 3 | buffer[14] << 5) & 0x07FF); // Channel 10
			checksum_l = buffer[25];

			// Calc checksum.
			checksum = roll_l + pitch_l + throttle_l + yaw_l + armed_l + angleMode_l + decoding_l + shutdown_l + failsafe_l + altMode_l + killswitch_l;
			checksum = checksum % 255; // Checksum is bit number.
			control_ok = (checksum == checksum_l);

			
			if (control_ok == 1)
			{
				//std::cout << "Good Data" << std::endl;
				lastArmed = armed_l;
				lastShutdown = shutdown_l;
				lastThrottle = throttle_l;
				lastRoll = roll_l;
				lastPitch = pitch_l;
				lastYaw = yaw_l;
				lastFailsafe = failsafe_l;
				lastAltMode = altMode_l;
				lastAngleMode = angleMode_l;
				lastDecoding = decoding_l;
				lastkillswitch = killswitch_l;
			}
			else
			{
				//std::cout << "Bad Data" << std::endl;
				armed_l = lastArmed;
				shutdown_l = lastShutdown;
				throttle_l = lastThrottle;
				roll_l = lastRoll;
				pitch_l = lastPitch;
				yaw_l = lastYaw;
				failsafe_l = lastFailsafe;
				altMode_l = lastAltMode;
				decoding_l = lastDecoding;
				angleMode_l = lastAngleMode;
				killswitch_l = lastkillswitch;
			}
			throttle.store(normalizethrot(throttle_l));
			roll.store(normalize(roll_l));
			pitch.store(normalize(pitch_l));
			yaw.store(normalize(yaw_l));
			angleMode.store((normalize(angleMode_l) > 50) ? true : false); //Channel 6
			decoding.store(((normalize(decoding_l) > 50) ? true : false));	//Channel 7
			killswitch.store(((normalize(killswitch_l) > 50 ? true : false))); // Channel 10
			if (!decoding)
			{
				killswitch.store(true);
			}
			if (failsafe_l == 1)
			{
				failsafe.store(true);
				delayMicroseconds(10 * 1000); // wait 10 millisecond until no failsafe anymore.
			}
			else
			{
				failsafe.store(false);
			}

			//Shutdown double control Channel 8
			if (normalize(shutdown_l) < 99)
			{
				shutdown.store(false);
				shutdown_control = false;
			}
			else if (!shutdown_control & (normalize(shutdown_l) > 99))
			{
				shutdown_control = true;
				shutdown.store(false);
			}
			else if (shutdown_control & (normalize(shutdown_l) > 99))
			{
				shutdown.store(true);
			}

			//Arm double control (both ways) Channel 5
			if (arm_control & (normalize(armed_l) > 50) & !failsafe.load())
			{
				armed.store(true);
			}
			else if (!arm_control & (normalize(armed_l) > 50) & !failsafe.load())
			{
				arm_control = true;
				armed.store(false);
			}
			else if (arm_control  & (normalize(armed_l) < 50))
			{
				arm_control = false;
				armed.store(true);
			}
			else //(!arm_control & (normalize(armed_l) < 50)) (Save time)
			{
				armed.store(false);
			}

			if (!decoding.load())
			{
				roll.store(0);
				pitch.store(0);
				yaw.store(0);
				throttle.store(50); //Set to stabilazation value!
				failsafe.store(true);

				//Make sure the copter was armed in the first place!
				if (arm_control)
				{
					//std::cout << "Armed stored" << std::endl;
					armed.store(true);
				}
				else
				{
					armed.store(false);
				}
				shutdown.store(false);
			}

			if ((buffer[23] >> 3) & 0x0001) {
				failsafe.store(true);
			}
			else {
				failsafe.store(false);
			}
			float altmode = normalize(altMode_l);
			if (altmode < 40)
			{
				altMode.store(0);
			}
			else if (altmode > 60)
			{
				altMode.store(2);
			}
			else
			{
				altMode.store(1);
			}
			now = getTimeU();
			proccessTime = now - then;
			//std::cout << "ProccessTime: " << (int)proccessTime.count() << std::endl;
			com.flushBuffer();
			if (proccessTime < period)
			{
				//std::cout << "RX thread sleeping" << std::endl;
				std::this_thread::sleep_for(period - proccessTime);
				//counter++;
			}
			then = getTimeU();
			//std::cout << counter << std::endl;
		}
	}
}

// Start the thread
void RXInput::start_RX_Input()
{
	std::cout << "Starting input thread" << std::endl;
	RXInputThread = std::thread(&RXInput::rxInputThread, this);
}

// Stop the thread
void RXInput::stop_RX_Input()
{
	RXInputThread.join();
}

// Return the inputs
void RXInput::getInputs(float *inputs)
{
	inputs[0] = roll.load();
	inputs[1] = pitch.load();
	inputs[2] = yaw.load();
	inputs[3] = throttle.load();
}

// Get receiver failsafe
bool RXInput::getFailsafe()
{
	return failsafe.load();
}

// Get the control mode selection
bool RXInput::getAngleMode()
{
	return angleMode.load();
}

// Get the arm status
bool RXInput::getArmed()
{
	return armed.load();
}

// Get system shutdown status
bool RXInput::getShutdown()
{
	return shutdown.load();
}

// Get altitude control mode
int RXInput::getAltMode()
{
	return altMode;
}

// Get kill switch state
bool RXInput::getKillSwitch()
{
	return killswitch;
}
