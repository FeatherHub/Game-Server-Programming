

#include "SelectNetwork.h"
#include "NetCode.h"

#include "..\..\Common\Util\BodySizeMananger.h"
#include "..\..\Common\Util\Logger.h"

namespace NNetworkLib
{
	bool SelectNetwork::Init(unsigned short port, const char* ip)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return false;

		m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
		if (m_listenSock == INVALID_SOCKET)
			return false;

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		inet_pton(AF_INET, ip, (void*)serverAddr.sin_addr.s_addr);

		if (bind(m_listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
			return false;

		unsigned long mode = 1;
		if (ioctlsocket(m_listenSock, FIONBIO, &mode) == SOCKET_ERROR)
			return false;

		InitClientStuff();

		if (listen(m_listenSock, SOMAXCONN) == SOCKET_ERROR)
			return false;

		FD_SET(m_listenSock, &m_fds);

		m_bodySizeMgr = new BodySizeManager();
		m_bodySizeMgr->Init();

		Logger::Write(Logger::INFO, "Server run");

		return true;
	}

	bool SelectNetwork::Run()
	{
		ProcessSelect();

		ProcessAccept();

		ProcessClient();

		return true;
	}

	bool SelectNetwork::ProcessSelect()
	{
		FD_ZERO(&m_readFds);
		FD_ZERO(&m_writeFds);

		m_readFds = m_fds;
		m_writeFds = m_fds;

		auto res = select(0, &m_readFds, &m_writeFds, nullptr, nullptr);

		if (res == SOCKET_ERROR)
			return false;

		return true;
	}

	NETCODE SelectNetwork::ProcessAccept()
	{
		if (m_clientNum >= FD_SETSIZE)
		{
			Logger::Write(Logger::WARN, "Client pool is full. Cannot accept.");
			return NETCODE::INFO_CLIENT_POOL_FULL;
		}

		if (FD_ISSET(m_listenSock, &m_readFds))
		{
			SOCKADDR_IN clientAddr;
			int addrLen = sizeof(clientAddr);
			SOCKET clientSock;
			int acceptCnt = 0;

			do
			{
				clientSock = accept(m_listenSock, (SOCKADDR*)&clientAddr, &addrLen);
				if (clientSock == INVALID_SOCKET)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						return NETCODE::INFO_ACCEPT_NONBLOCKED;
					}

					return NETCODE::ERROR_ACCEPT_SOCKET;
				}

				AddClient(clientSock, clientAddr);
			} while (++acceptCnt < MAX_ACCEPT_AT_ONCE &&
					m_clientNum < FD_SETSIZE);
		}

		return NETCODE::NONE;
	}

	NETCODE SelectNetwork::ProcessClient()
	{
		for (int id = 0; id < FD_SETSIZE; id++)
		{
			Client& client = m_clientPool[id];

			if (client.IsConnected())
			{
				if (FD_ISSET(client.s, &m_readFds))
				{
					auto res = Recv(id);
					if (res == NETCODE::INFO_CLIENT_LEFT)
					{
						CloseClient(id);

						continue;
					}

					RecvBuffProc(id);
				}

				if (FD_ISSET(client.s, &m_writeFds))
				{
					Send(id);

					SendBuffProc(id);
				}
			}
		}

		return NETCODE::NONE;
	}

	NETCODE SelectNetwork::Recv(int id)
	{
		Client& c = m_clientPool[id];

		CopyMemory(c.recvBuff, c.recvBuff + c.readPos, c.recvSize);
		c.readPos = 0;

		int res = recv(c.s, c.recvBuff + c.recvSize,
			Client::MAX_BUFF_SIZE - c.recvSize, 0);

		if (res == 0)
		{
			return NETCODE::INFO_CLIENT_LEFT;
		}
		else if (res < 0) //SOCKET_ERROR
		{
			return NETCODE::ERROR_RECV_SOCKET;
		}
		else
		{
			c.recvSize += res;
		}

		return NETCODE::NONE;
	}

	void SelectNetwork::RecvBuffProc(int clientId)
	{
		Client& c = m_clientPool[clientId];

		while (true)
		{
			if (c.recvSize < PACKET_HEADER_SIZE)
			{
				break;
			}

			PktHeader* pktH = (PktHeader*)(c.recvBuff + c.readPos);

			int bodySize = m_bodySizeMgr->Get(pktH->id);

			if (c.recvSize < PACKET_HEADER_SIZE + bodySize)
			{
				break;
			}

			char* dataPos = c.recvBuff + c.readPos + PACKET_HEADER_SIZE;

			AddToRecvPktQueue(RecvPacket{ pktH->id, dataPos, clientId });

			c.readPos += PACKET_HEADER_SIZE + bodySize;
			c.recvSize -= PACKET_HEADER_SIZE + bodySize;
		}
	}

	NETCODE SelectNetwork::Send(int id)
	{
		Client& c = m_clientPool[id];

		int sentSize = 0;
		sentSize = send(c.s, c.sendBuff, c.sendSize, 0);
		if (sentSize < 0)
		{
			return NETCODE::ERROR_SEND_SOCKET;
		}
		else
		{
			c.sentSize = sentSize;
		}

		return NETCODE::NONE;
	}

	bool SelectNetwork::SendBuffProc(int id)
	{
		Client& c = m_clientPool[id];

		CopyMemory(c.sendBuff, c.sendBuff + c.sentSize, c.sendSize - c.sentSize);

		c.sendSize -= c.sentSize;

		return true;
	}

	RecvPacket SelectNetwork::GetPacket()
	{
		RecvPacket pkt = m_recvPktQueue.front();
		m_recvPktQueue.pop();

		return pkt;
	}

	void SelectNetwork::AddToRecvPktQueue(RecvPacket packet)
	{
		m_recvPktQueue.push(packet);
	}

	NNetworkLib::NETCODE SelectNetwork::SendPacket(int receiverId, Packet&& packet)
	{
		Client& c = m_clientPool[receiverId];
		int bodySize = m_bodySizeMgr->Get(packet.id);

		if (c.sendSize + PACKET_HEADER_SIZE + bodySize > Client::MAX_BUFF_SIZE)
		{
			return NETCODE::ERROR_SENDBUFFER_FULL;
		}

		CopyMemory(c.sendBuff + c.sendSize, &packet, PACKET_HEADER_SIZE + bodySize);

		return NETCODE::NONE;
	}

	void SelectNetwork::AddClient(SOCKET s, SOCKADDR_IN& addr)
	{
		int id = m_clientIndexPool.front();
		m_clientIndexPool.pop();

		m_clientPool[id].s = s;
		inet_ntop(AF_INET, &(addr.sin_addr.s_addr), m_clientPool[id].IP, INET_ADDRSTRLEN);

		FD_SET(s, &m_fds);

		Logger::Write(Logger::INFO, "Client %s connected", m_clientPool[id].IP);

		m_clientNum++;
	}

	void SelectNetwork::CloseClient(int id)
	{
		auto& target = m_clientPool[id];

		FD_CLR(target.s, &m_fds);

		closesocket(target.s);
		target.recvSize = 0;
		target.sendSize = 0;

		m_clientIndexPool.push(id);

		m_clientNum--;

		Logger::Write(Logger::INFO, "Client %s has left", target.IP);
	}

	void SelectNetwork::InitClientStuff()
	{
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			m_clientIndexPool.push(i);
		}

		m_clientNum = 0;
	}

	bool SelectNetwork::PacketQueueEmpty()
	{
		return m_recvPktQueue.empty();
	}

	SelectNetwork::~SelectNetwork()
	{
		WSACleanup();
	}
}