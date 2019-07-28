#include "TCPComm.h"

TCPComm::TCPComm() 
{
	finishServer.store(false);
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
	if (serverSocketFd <= 0)
	{
		std::cerr << "ERROR: Server socket not running!" << std::endl;
		return;
	}

	listen(serverSocketFd, 5);
	std::cout << "Server listening" << std::endl;

	while (!finishServer)
	{
		int newSocketFd = accept(serverSocketFd,(struct sockaddr*)NULL, NULL);
		std::cout << "New incoming client" << std::endl;
		// TODO communicate link.
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
	int requestNum = 0;

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
		int rlen = read(socketfd, buffer, 255);
		if (rlen <= 0)
		{
			closeSocket();
			continue;
		}
		if (rlen > 1)
		{
			char wbuf[1];
			wbuf[0] = 0;
			if (!write(socketfd, wbuf, 1))
			{
				closeSocket();
				continue;
			}
		}
		requestNum++;
		float data[10] = { 1.2588654486413,2,3,4,5,6,7,8,9,10 };
		std::string response = buildMessageData(data, 10);
		std::cout << response << std::endl;
		const char* wbuf = response.c_str();
		int count = write(socketfd, wbuf, strlen(wbuf));
		if (!count)
		{
			closeSocket();
			continue;
		}
	}
}

std::string TCPComm::buildMessageData(float* data, size_t length)
{
	std::ostringstream message;
	for (int i = 0; i < length; i++)
		message << std::setprecision(15) <<data[i] << "|";
	return message.str();
}

void TCPComm::fetchLoggingData(float* data)
{
	return;
}

void TCPComm::writePIDData()
{
	return;
}
