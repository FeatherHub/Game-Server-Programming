#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <queue>

#include "..\..\Common\Packet.h"

enum NETCODE : int;

class BodySizeManager;

class Network
{
public:
	Network();
	bool Init();
	bool Connect(char* ip, unsigned short port);
	void Run();
	NETCODE SendPacket(unsigned short pktId, char* pData);
	Packet GetPacket();
	bool PktQueueEmpty() { return m_pktQueue.empty(); }
	void CloseConnect();

private:
	NETCODE Select();
	NETCODE Recv();
	void RecvBuffProc();
	NETCODE Send();
	void SendBuffProc();

	void AddToPktQueue(Packet&& pkt);

private:
	SOCKET m_socket;
	fd_set m_readFd;
	fd_set m_writeFd;

	std::queue<Packet> m_pktQueue;

	bool m_isConnected;

	BodySizeManager* m_bodySizeMgr;
	enum Config
	{ 
		MAX_BUFF_SIZE = 1024, 
		SELECT_WAIT_SEC = 0, 
		SELECT_WAIT_MILLSEC = 100 
	};

	char m_recvBuff[MAX_BUFF_SIZE];
	int m_readPos;
	int m_recvSize;

	char m_sendBuff[MAX_BUFF_SIZE];
	int m_sendSize;
	int m_sentSize;
};