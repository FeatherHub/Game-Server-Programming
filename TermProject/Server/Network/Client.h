#pragma once
namespace NNetworkLib
{
	class Client
	{
	public:
		bool IsConnected() { return s > 0 ? true : false; }

	public:
		enum { MAX_BUFF_SIZE = 1024 };

	public:
		unsigned int s;
		char IP[32];

		char recvBuff[MAX_BUFF_SIZE];
		int recvSize = 0;
		int readPos = 0;

		char sendBuff[MAX_BUFF_SIZE];
		int sendSize = 0;
		int sentSize = 0;
	};
}