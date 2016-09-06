#pragma once

#include <Winsock2.h>
#include <WS2tcpip.h> //꼭 Winsock2과 함께 include한다!!!
#include <queue>

#include "PacketQueue.h"
#include "NetData.h"

class IOCPNetwork
{
public:
	IOCPNetwork()  = default;
	~IOCPNetwork() = default;
	ERROR_CODE			Init(const char* IP, const short port);
	ERROR_CODE			Run();

private:
	void				AcceptThreadFunc();
	void				RecvThreadFunc();
	void				SendThreadFunc();

	HANDLE				CreateIOCP();
	void				BindSocketToCP(SOCKET sock);
	void				InitClientStuff();

	void				InitClient(Client& target, SOCKET sock, SOCKADDR_IN* addr);
	void				CloseClient(int clientIdx);
	ERROR_CODE			HandleError(const char* msg);

private:
	std::queue<int>		m_clientIdxPool;
	std::mutex			m_idxPoolMutex;
	Client				m_clientPool[(int)NetworkConfig::MAX_CLIENT_NUM];
	std::mutex			m_clientPoolMutex;
	int					m_clientNum = 0;

	PacketQueue			m_recvPktQueue;
	PacketQueue			m_sendPktQueue;

	SOCKET				m_listenSock;

	HANDLE				m_ioComPort;

	bool				m_initialized = false;
	bool				m_isRunning = false;
};