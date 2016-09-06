#pragma once

#include <Winsock2.h>

enum class NETCODE : int
{
	SOCKET_FUNC_ERROR = -1,
	NONE = 0,
	INIT_NOT_CALLED = 10,
	ALREADY_RUNIING = 11,
};

enum class NetConfig : int 
{
	MAX_CLIENT_NUM = 1024,
	MAX_BUFF_SIZE = 512,
	MAX_IP_LEN = 32,
};

class Client
{
public:
	bool IsConnected() { return isConnected; }

public:
	unsigned long long socket;
	bool isConnected = false;
	char IP[(int)NetConfig::MAX_IP_LEN];

	char recvBuff[(int)NetConfig::MAX_BUFF_SIZE];
	int recvSize = 0;
	int readPos = 0;

	char sendBuff[(int)NetConfig::MAX_BUFF_SIZE];
	int sendSize = 0;
	int sentSize = 0;
};

enum class RW_MODE : int
{
	READ,
	WRITE,
};

class ClientData
{
public:
	ClientData() = default;

	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	int clientIdx;
	RW_MODE rwMode;
};