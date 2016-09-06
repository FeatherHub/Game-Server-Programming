#pragma once

enum class ERROR_CODE : int
{
	SOCKET_FUNC_ERROR = -1,
	NONE = 0,
	INIT_NOT_CALLED = 10,
	ALREADY_RUNIING = 11,
};

enum class NetworkConfig : int 
{
	MAX_CLIENT_NUM = 1024,
	MAX_BUFF_SIZE = 512,
};

struct RecvPacket
{
	int clientIdx = 0;
	short packetId = 0;
	char* pData = nullptr;
};

class Client
{
public:
	bool IsConnected() { return isConnected; }

public:
	enum { MAX_BUFF_SIZE = 2048 };

public:
	unsigned int socket;
	bool isConnected = false;
	char IP[32];

	char recvBuff[MAX_BUFF_SIZE];
	int recvSize = 0;
	int readPos = 0;

	char sendBuff[MAX_BUFF_SIZE];
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