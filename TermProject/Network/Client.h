#pragma once
namespace NNetworkLib
{
	class Client
	{
	public:
		void Disconnect() { isConnected = false; }
		void Connect() { isConnected = true; }
		bool IsConnected() { return isConnected; }

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

	private:
		bool isConnected = false;
	};
}