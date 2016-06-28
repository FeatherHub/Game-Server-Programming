#include "TCPNetwork.h"

namespace TCPServerLib
{
	TCPNetwork::TCPNetwork() { }

	TCPNetwork::~TCPNetwork()
	{
		for (auto& client : m_clientSessionPool)
		{
			if (client.pRecvBuffer)
			{
				delete[] client.pRecvBuffer;
			}

			if (client.pSendBuffer)
			{
				delete[] client.pSendBuffer;
			}
		}
	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::Init(ServerConfig* pConfig, ILog* pLogger)
	{
		NET_ERROR_CODE ret;

		m_pConfig = pConfig;
		
		m_pLogger = pLogger;

		//Server socket setting 1
		ret = InitServerPacket();
		if (ret != NET_ERROR_CODE::NONE)
		{
			return ret;
		}

		//Server socket setting 2
		ret = BindListen(m_pConfig->Port, m_pConfig->BackLogCount);
		if (ret != NET_ERROR_CODE::NONE)
		{
			return ret;
		}

		//File descriptor set for select
		FD_ZERO(&m_readSet);
		FD_SET(m_serverSocket, &m_readSet);

		//Create client information vector for maintaining and managing
		CreateSessionPool(m_pConfig->MaxClientCount + m_pConfig->ExtraClientCount);

		return NET_ERROR_CODE::NONE;
	}

	void TCPNetwork::Run()
	{
		//Set select parameters 
		auto readSet = m_readSet;
		auto writeSet = m_readSet;
		auto excptSet = m_readSet;
		timeval timeOut{ 0, 1000 };

		//Call select and check result
		auto selectRet = select(0, &readSet, &writeSet, &excptSet, &timeOut);
		auto isFDSetChanged = RunCheckSelectResult(selectRet);
		if (isFDSetChanged == false)
		{
			return;
		}

		//Process FD elements
		if (FD_ISSET(m_serverSocket, &readSet))
		{
			NewSession();
		}
		else
		{
			RunCheckSelectClient(readSet, writeSet, excptSet);
		}
	}

	NET_ERROR_CODE TCPNetwork::SendData(const int sIdx, const short pktId, const short size, const char* pMsg)
	{ //분석해보기
		auto& cs = m_clientSessionPool[sIdx];

		auto pos = cs.SendSize;

		if ((pos + size + PACKET_HEADER_SIZE) > m_pConfig->MaxClientSendBufferSize)
		{
			return NET_ERROR_CODE::CLIENT_SEND_BUFFER_FULL;
		}

		PacketHeader pktHeader{ pktId, size };
		
		memcpy(cs.pSendBuffer + pos, (char*)&pktHeader, PACKET_HEADER_SIZE);
		memcpy(cs.pSendBuffer + pos + PACKET_HEADER_SIZE, pMsg, size);
		cs.SendSize += (size + PACKET_HEADER_SIZE);

		return NET_ERROR_CODE::NONE;
	}

	RecvPacketInfo TCPNetwork::GetPacketInfo()
	{
		RecvPacketInfo recvPktInfo;

		if (m_packetQueue.empty() == false)
		{
			recvPktInfo = m_packetQueue.front();
			m_packetQueue.pop_front();
		}

		return recvPktInfo;
	}

	NET_ERROR_CODE TCPNetwork::InitServerPacket()
	{
		int ret;

		WSADATA wsaData;
		ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret)
		{
			//handle exception
		}

		m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_serverSocket < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		int n = 1;
		ret = setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n));
		if (ret < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		}

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TCPNetwork::BindListen(short port, int backlogCount)
	{
		int ret;

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(port);

		ret = bind(m_serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (ret < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
		}

		ret = listen(m_serverSocket, backlogCount);
		if (ret == SOCKET_ERROR)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_FAIL;
		}

		return NET_ERROR_CODE::NONE;
	}

	void TCPNetwork::CreateSessionPool(const int maxClientCount)
	{
		auto maxRecvBufSize = m_pConfig->MaxClientRecvBufferSize;
		auto maxSendBufSize = m_pConfig->MaxClientSendBufferSize;

		for (int i = 0; i < maxClientCount; i++)
		{
			ClientSession cs;
			ZeroMemory(&cs, sizeof(ClientSession));
			cs.Index = i;
			cs.pRecvBuffer = new char[maxRecvBufSize];
			cs.pSendBuffer = new char[maxSendBufSize];
			
			m_clientSessionPool.push_back(cs);
			m_clientSessionPoolIndex.push_back(cs.Index);
		}
	}

	bool TCPNetwork::RunCheckSelectResult(const int result)
	{
		if (result == 0)
		{
			return false;
		}
		else if (result == -1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::NewSession()
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET clientSockFd;

		clientSockFd = accept(m_serverSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSockFd < 0)
		{
			return NET_ERROR_CODE::ACCEPT_API_ERROR;
		}

		int newSessionIdx = GetClientSessionIndex();
		if (newSessionIdx < 0)
		{
			CloseSession(SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY, clientSockFd, -1);
			return NET_ERROR_CODE::ACCEPT_MAX_SESSION_COUNT;
		}

		SetSockOption(clientSockFd);

		FD_SET(clientSockFd, &m_readSet);

		ConnectedSession(newSessionIdx, clientSockFd);

		return NET_ERROR_CODE::NONE;
	}

	int TCPNetwork::GetClientSessionIndex()
	{
		if (m_clientSessionPoolIndex.empty())
		{
			return -1;
		}

		auto idx = m_clientSessionPoolIndex.front();
		m_clientSessionPoolIndex.pop_front();

		return idx;
	}

	void TCPNetwork::SetSockOption(SOCKET s)
	{
		linger ling;
		ling.l_linger = 0;
		ling.l_onoff = 0;

		setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));

		int maxRecvBufSize = m_pConfig->MaxClientRecvBufferSize;
		int maxSendBufSize = m_pConfig->MaxClientSendBufferSize;

		setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&maxRecvBufSize, sizeof(int));
		setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&maxSendBufSize, sizeof(int));
	}

	void TCPNetwork::ConnectedSession(int sessionIdx, int sockFd)
	{
		m_connectSeq++;

		auto& cs = m_clientSessionPool[sessionIdx];
		cs.Seq = m_connectSeq;
		cs.SocketFD = sockFd;

		m_connectedSessionCount++;
	}

	void TCPNetwork::RunCheckSelectClient(fd_set& read_set, fd_set& write_set, fd_set& exc_set)
	{
		int size = m_clientSessionPool.size();

		for (int i = 0; i < size; i++)
		{
			auto& cs = m_clientSessionPool[i];

			if (cs.IsConnected() == false)
			{
				continue;
			}

			SOCKET sock = cs.SocketFD;
			int sIdx = cs.Index;

			if (FD_ISSET(sock, &exc_set) == true)
			{
				CloseSession(SOCKET_CLOSE_CASE::SELECT_ERROR, sock, sIdx);
				continue;
			}

			auto ret = RunProcessReceive(sIdx, sock, read_set);
			if (ret == false)
			{
				continue;
			}

			ret = RunProcessWrite(sIdx, sock, write_set);
		} //end for loop
	}

	bool TCPNetwork::RunProcessReceive(const int sessionIdx, const SOCKET fd, fd_set& read_set)
	{
		if (FD_ISSET(fd, &read_set))
		{
			return true;
		}

		auto ret = ReceiveSocket(sessionIdx);
		if (ret != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_ERROR, fd, sessionIdx);
			return false;
		}

		ret = ReceiveBufferProcess(sessionIdx);
		if (ret != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_BUFFER_PROCESS_ERROR, fd, sessionIdx);
			return false;
		}

		return true;
	}

	NET_ERROR_CODE TCPNetwork::ReceiveSocket(const int sessionIdx)
	{ //분석해보기!
		auto& cs = m_clientSessionPool[sessionIdx];
		SOCKET fd = static_cast<SOCKET>(cs.SocketFD);

		if (cs.IsConnected() == false)
		{
			return NET_ERROR_CODE::RECV_PROCESS_NOT_CONNECTED;
		}

		int recvPos = 0;
		if (cs.RemainingDataSize > 0)
		{
			recvPos += cs.RemainingDataSize;
			cs.RemainingDataSize = 0;
		}

		int recvSize = recv(fd, (cs.pRecvBuffer + recvPos), (MAX_PACKET_SIZE * 2), 0);
		if (recvSize == 0)
		{
			return NET_ERROR_CODE::RECV_REMOTE_CLOSE;
		}

		if (recvSize < 0)
		{
			auto error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				return NET_ERROR_CODE::RECV_API_ERROR;
			}
			else
			{
				return NET_ERROR_CODE::NONE;
			}
		}

		cs.RemainingDataSize += recvSize;

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TCPNetwork::ReceiveBufferProcess(const int sessionIdx)
	{ //분석해보기!
		auto& cs = m_clientSessionPool[sessionIdx];

		auto readPos = 0;
		auto dataSize = cs.RemainingDataSize;
		PacketHeader* pPktHeader;

		while ((dataSize - readPos) >= PACKET_HEADER_SIZE)
		{
			pPktHeader = (PacketHeader*)&cs.pRecvBuffer[readPos];
			readPos += PACKET_HEADER_SIZE;

			if (pPktHeader->BodySize > 0)
			{
				if (pPktHeader->BodySize < (dataSize - readPos))
				{
					break;
				}

				if (pPktHeader->BodySize > MAX_PACKET_SIZE)
				{
					return NET_ERROR_CODE::RECV_CLIENT_MAX_PACKET;
				}
			}

			AddPacketQueue(sessionIdx, pPktHeader->Id, pPktHeader->BodySize, (cs.pRecvBuffer + readPos));

			readPos += pPktHeader->BodySize;
		}
	}

	bool TCPNetwork::RunProcessWrite(const int sessionIdx, const SOCKET fd, fd_set& write_set)
	{
		if (FD_ISSET(fd, &write_set) == false)
		{
			return true;
		}

		auto retSend = FlushSendBuffer(sessionIdx);
		if (retSend != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_SEND_ERROR, fd, sessionIdx);
			return false;
		}

		return true;
	}

	NET_ERROR_CODE TCPNetwork::FlushSendBuffer(const int sessionIdx)
	{ //분석해보기
		auto& cs = m_clientSessionPool[sessionIdx];
		auto fd = static_cast<SOCKET>(cs.SocketFD);

		if (cs.IsConnected() == false)
		{
			return NET_ERROR_CODE::CLIENT_FLUSH_SEND_BUFF_REMOTE_CLOSE;
		}

		auto ret = SendSocket(fd, cs.pSendBuffer, cs.SendSize);
		if (ret != NET_ERROR_CODE::NONE)
		{
			return ret;
		}

		int sendSize = static_cast<int>(ret);
		if (sendSize < cs.SendSize)
		{
			memmove(cs.pSendBuffer, 
				cs.pRecvBuffer + sendSize,
				cs.SendSize - sendSize);

			cs.SendSize -= sendSize;
		}
		else
		{
			cs.SendSize = 0;
		}

		return ret;
	}

	NET_ERROR_CODE TCPNetwork::SendSocket(const SOCKET fd, const char* pMsg, const int size)
	{
		if (size <= 0)
		{
			return NET_ERROR_CODE::NONE;
		}

		auto ret = send(fd, pMsg, size, 0);
		if (ret <= 0)
		{
			return NET_ERROR_CODE::SEND_SIZE_ZERO;
		}

		return static_cast<NET_ERROR_CODE>(ret);
	}

	void TCPNetwork::AddPacketQueue(const int sessionIdx, const short pktId, const short bodySize, char* pDataPos)
	{
		RecvPacketInfo recvPktInfo;
		recvPktInfo.SessionIndex = sessionIdx;
		recvPktInfo.PacketId = pktId;
		recvPktInfo.PacketBodySize = bodySize;
		recvPktInfo.pRefData = pDataPos;

		m_packetQueue.push_back(recvPktInfo);
	}

	void TCPNetwork::ReleaseSessionIndex(const int idx)
	{
		m_clientSessionPoolIndex.push_back(idx);
	}

	void TCPNetwork::CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sock, const int sessionIdx)
	{
		if (closeCase == SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY)
		{
			closesocket(sock);
			FD_CLR(sock, &m_readSet);

			return;
		}

		if (m_clientSessionPool[sessionIdx].IsConnected() == false)
		{
			return;
		}

		closesocket(sock);
		FD_CLR(sock, &m_readSet);

		m_clientSessionPool[sessionIdx].Clear();
		m_connectedSessionCount--;
		ReleaseSessionIndex(sessionIdx);

		AddPacketQueue(sessionIdx, (short)PACKET_ID::NTF_SYS_CLOSE_SESSION, 0, nullptr);
	}
}