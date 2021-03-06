#pragma once
namespace NNetworkLib
{
	class Client
	{
	public:
		bool IsConnected() { return isConnected; }

	public:
		enum { MAX_BUFF_SIZE = 2048 };

	public:
		unsigned long long s;
		bool isConnected = false;
		char IP[32];

		char recvBuff[MAX_BUFF_SIZE];
		int recvSize = 0;
		int readPos = 0;

		char sendBuff[MAX_BUFF_SIZE];
		int sendSize = 0;
		int sentSize = 0;
	};
}