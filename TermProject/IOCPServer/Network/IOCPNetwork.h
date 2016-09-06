#pragma once

#include <Winsock2.h>
#include <WS2tcpip.h> //꼭 Winsock2과 함께 include한다!!!
#include <queue>

#include "NetPacketQueue.h"
#include "NetData.h"

class BodySizeManager;

class IOCPNetwork
{
	using NetPacket = NetworkLib::NetPacket;
public:
	IOCPNetwork()  = default;
	~IOCPNetwork() = default;
	NETCODE Init(const short port);
	NETCODE				Run();
	
	void				SendPacket(int clientIdx, unsigned short pktid, char* dataPos);
	NetPacket			GetPacket();
	bool				PacketQueueEmpty();

	std::queue<int>&	GetClosedClientQueue();
	std::mutex&			GetClosedClientQueueMutex();
	void				CloseClient(int clientIdx);

private:
	void				AcceptThreadFunc();
	void				RecvThreadFunc();
	void				SendThreadFunc();

	HANDLE				CreateIOCP();
	void				BindSocketToCP(SOCKET sock);
	void				InitClientStuff();

	void				InitClient(Client& target, SOCKET sock, SOCKADDR_IN* addr, bool isConnected);
	NETCODE				HandleError(const char* msg);

private:
	std::queue<int>		m_clientIdxPool;
	std::mutex			m_idxPoolMutex;
	std::queue<int>		m_closedIdxPool;
	std::mutex			m_closedPoolMutex;

	Client				m_clientPool[(int)NetConfig::MAX_CLIENT_NUM];
	std::mutex			m_clientPoolMutex;
	int					m_clientNum = 0;

	NetPacketQueue		m_recvPktQueue;
	NetPacketQueue		m_sendPktQueue;

	BodySizeManager*	m_bodySizeMgr;
	std::mutex			m_bodySizeMgrMutex;

	SOCKET				m_listenSock;

	HANDLE				m_ioComPort;

	bool				m_initialized = false;
	bool				m_isRunning = false;
};