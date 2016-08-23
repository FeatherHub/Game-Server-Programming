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
	static Network* GetInstance();
private:
	Network();
	static Network* m_instance;

public:
	bool Init();

	void Run();
	
	bool ConnectTo(const char* ip, unsigned short port);
	void CloseConnect();

	NETCODE SendPacket(unsigned short pktId, char* pData);
	Packet GetPacket();
	bool PktQueueEmpty() { return m_pktQueue.empty(); }

private:
	NETCODE Select();

	NETCODE Recv();
	void RecvBuffProc();

	NETCODE Send();
	void SendBuffProc();

	void AddToPktQueue(Packet&& pkt);

	void DisplayErrorMsg(const char* msg);

private:
	SOCKET m_socket;
	fd_set m_readFd;
	fd_set m_writeFd;
	
	std::queue<Packet> m_pktQueue;

	BodySizeManager* m_bodySizeMgr;

	enum Config
	{ 
		MAX_BUFF_SIZE = 1024, 
		SELECT_WAIT_SEC = 0, 
		SELECT_WAIT_MILLSEC = 100 
	};

	bool m_isConnected = false;

	char m_recvBuff[MAX_BUFF_SIZE] = { 0, };
	int m_readPos = 0;
	int m_recvSize = 0;

	char m_sendBuff[MAX_BUFF_SIZE] = { 0, };
	int m_sendSize = 0;
	int m_sentSize = 0;
};