#pragma once

#define FD_SETSIZE 1024
#include <WinSock2.h>

#include <queue>

#include "Client.h"
#include "NetPacket.h"
#include "NetCode.h"

namespace NNetworkLib
{
	class Client;

	//Subject : Server
	//Transport : TCP
	//Socket I/O Model : Select

	class SelectNetwork
	{
	public:
		SelectNetwork() = default;
		~SelectNetwork();
		bool Init(unsigned short port, const char* ip);
		bool Run();

		NETCODE SendPacket(int receiverId, Packet& packet);
		RecvPacket GetPacket();
		bool PacketQueueEmpty() { return m_recvPktQueue.empty(); }

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
		NETCODE Send(int id);
		bool SendBuffProc(int id);

		void AddToRecvPktQueue(RecvPacket&& packet);

		void CloseClient(int id);

	private:
		Client m_clientPool[FD_SETSIZE];
		std::queue<int> m_clientIndexPool;
		int m_clientNum = 0;

		fd_set m_fds;

		std::queue<RecvPacket> m_recvPktQueue;

		SOCKET m_listenSock;

	private:
		enum 
		{ 
			MAX_ACCEPT_AT_ONCE = 30,
			MAX_CLIENT_NUM = FD_SETSIZE
		};
	};
}