#pragma once

#define FD_SETSIZE 1024
#include <WinSock2.h>

#include <queue>

#include "Client.h"
#include "Packet.h"
#include "NetCode.h"

class Client;

//Subject : Server
//Transport : TCP
//Socket I/O Model : Select

class Network
{
public:
	Network() = default;
	~Network();
	bool Init(unsigned short port, const char* ip);
	bool Run();

	NETCODE SendPacket(int id, Packet& packet);
	Packet GetPacket();

private:
	//INIT//
	void InitClientStuff();

	//RUN//
	bool ProcessSelect();

	NETCODE ProcessAccept();
	void AddClient(SOCKET s, SOCKADDR_IN& addr);

	NETCODE ProcessClient();
	NETCODE Recv(int id);
	void RecvBuffProc(int id);
	bool Send(int id);
	bool SendBuffProc(int id);

	void AddToRecvPktQueue(Packet&& packet);

	void CloseClient(int id);

private:
	Client m_clientPool[FD_SETSIZE];
	std::queue<int> m_clientIndexPool;
	const int MAX_CLIENT_NUM = FD_SETSIZE;
	int m_clientNum = 0;

	fd_set m_writeFds;
	fd_set m_readFds;

	std::queue<Packet> m_recvPktQueue;

	SOCKET m_listenSock;
};