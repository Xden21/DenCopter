#include "TCPComm.h"

static void splitStringToFloat(std::string* input, std::vector<float>* out)
{
	std::vector<float> numbers;
	size_t pos = 0;
	while ((pos = input->find("|")) != std::string::npos)
	{
		numbers.push_back(std::stof(input->substr(0, pos)));
		input->erase(0, pos + 1); // delete previous including delimiter
	}
}

TCPComm::TCPComm() 
{
	finishServer.store(false);
	clientAmount = 0;
	PIDDataAvailable = false;
}

TCPComm::~TCPComm()
{
	stopLoggingServer();
}

void TCPComm::setupLoggingServer()
{
	struct sockaddr_in serv_addr;
	serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketFd < 0)
	{
		std::cerr << "ERROR: Unable to set up server!" << std::endl;
	}
	bzero((char*)& serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	if (bind(serverSocketFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "ERROR: Unable to bind to server!" << std::endl;
	}
}

void TCPComm::startLoggingServer()
{
	loggingServerThread = std::thread(&TCPComm::loggingServer, this);
}

void TCPComm::loggingServer() 
{
	if (serverSocketFd <= 0)
	{
		std::cerr << "ERROR: Server socket not running!" << std::endl;
		return;
	}

	listen(serverSocketFd, 5);
	std::cout << "Server listening" << std::endl;

	char buffer[20];
	while (!finishServer)
	{
		int newSocketFd = accept(serverSocketFd,(struct sockaddr*)NULL, NULL);
		bzero(buffer, 20);
		// Wait for id message
		int rlen = read(newSocketFd, buffer, 20);
		std::string idMessage(buffer);
		if (idMessage != std::string("DencopterClient"))
		{
			// Bad id
			const char wbuf[1] = { 2 };
			write(newSocketFd, wbuf, strlen(wbuf));
			close(newSocketFd);
			continue;
		}
		else if ((clientAmount + 1) > MAX_CLIENTS)
		{
			// Too much clients
			const char wbuf[1] = { 3 };
			write(newSocketFd, wbuf, strlen(wbuf));
			close(newSocketFd);
			continue;
		}
		clientAmount++;
		std::cout << "New incoming client" << std::endl;
		// Confirm connection.
		const char wbuf[1] = { 1 };
		write(newSocketFd, wbuf, strlen(wbuf));
		loggerThreads.push_back(std::thread(&TCPComm::loggerThread, this, &newSocketFd));
	}
}

void TCPComm::stopLoggingServer()
{
	finishServer.store(true);
	for (auto& t : loggerThreads)
	{
		if (t.joinable())
			t.join();
	}
}

void TCPComm::loggerThread(void* newsocketfd)
{
	int socketfd = *(int*)newsocketfd;
	char buffer[256];
	bool finish = false;
	bool firstTime = true;

	auto closeSocket = [&]()
	{
		close(socketfd);
		finish = true;
	};
	while (!finish)
	{
		if (finishServer.load())
		{
			closeSocket();
			continue;
		}
		int rlen = read(socketfd, buffer, 255); // Data request came in.
		if (rlen <= 0)
		{
			closeSocket();
			continue;
		}

		if (buffer[0] == 1 || buffer[0] == 11)
		{
			std::string message = "OK||";
			if (buffer[0] == 11)
			{
				// PID also came in.
				std::string data(buffer);
				std::vector<float> PidData;
				data = data.substr(1);
				splitStringToFloat(&data, &PidData);

				std::unique_lock<std::mutex> PIDLock(PIDMutex);
				for (int i = 0; i < PID_DATA_COUNT; i++)
				{
					PIDData_Raw[i] = PidData[i];
				}

				PIDDataAvailable = true;
				PIDLock.unlock();
				PIDSignal.notify_one();
				message = "PID||";
				// So other connected clients get updated values too.
			}
			if (firstTime)
			{
				// Write PID data, so client knows actual values
				firstTime = false;
				message = "PID||";
			}
			// So the array isn't changed during message build.
			std::unique_lock<std::mutex> lock(LogDataMutex);
			message += buildMessageData(loggingData, logAmount);
			lock.release();

			// Append (new) PID data
			message += PIDMode + "|";
			std::unique_lock<std::mutex> PIDLock(PIDMutex);
			std::string PIDMessage = buildMessageData(PIDData_Raw, PID_DATA_COUNT);
			PIDLock.unlock();
			message += PIDMessage;

			// Send message.
			const char* wbuf = message.c_str();
			int count = write(socketfd, wbuf, strlen(wbuf));
			if (!count)
			{
				closeSocket();
				continue;
			}
		}
		else
		{
			std::string message = "BAD||";
			// Send message.
			const char* wbuf = message.c_str();
			int count = write(socketfd, wbuf, strlen(wbuf));
			if (!count)
			{
				closeSocket();
				continue;
			}
		}
	}
	// Thread still exists but is finished, so make space for new one.
	clientAmount--;
}

std::string TCPComm::buildMessageData(float* data, size_t length)
{
	std::ostringstream message;
	for (int i = 0; i < length; i++)
		message << std::setprecision(15) << data[i] << "|";
	return message.str();
}

void TCPComm::setLoggingData(float* data, size_t amount)
{
	std::unique_lock<std::mutex> lock(LogDataMutex);
	logAmount = amount;
	loggingData = data;
	return;
}

void TCPComm::getAttitudePIDData(Attitude_PIDData* data)
{
	// Mutex should be locked before entering function
	data->mode = PIDMode;
	data->yaw_KP = PIDData_Raw[0];
	data->yaw_KI = PIDData_Raw[1];
	data->yaw_KD = PIDData_Raw[2];
	data->pitch_KP = PIDData_Raw[3];
	data->pitch_KI = PIDData_Raw[4];
	data->pitch_KD = PIDData_Raw[5];
	data->roll_KP = PIDData_Raw[6];
	data->roll_KI = PIDData_Raw[7];
	data->roll_KD = PIDData_Raw[8];
}

void TCPComm::setAttitudePIDData(Attitude_PIDData* data)
{
	PIDMode = data->mode;
	PIDData_Raw[0] = data->yaw_KP;
	PIDData_Raw[1] = data->yaw_KI;
	PIDData_Raw[2] = data->yaw_KD;
	PIDData_Raw[3] = data->pitch_KP;
	PIDData_Raw[4] = data->pitch_KI;
	PIDData_Raw[5] = data->pitch_KD;
	PIDData_Raw[6] = data->roll_KP;
	PIDData_Raw[7] = data->roll_KI;
	PIDData_Raw[8] = data->roll_KD;
}
