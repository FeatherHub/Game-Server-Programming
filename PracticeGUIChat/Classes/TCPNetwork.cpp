#include "TCPNetwork.h"

TCPNetwork::TCPNetwork()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		exit(1);
	}
}

TCPNetwork::~TCPNetwork()
{
	WSACleanup();
}

bool TCPNetwork::ConnectTo(const char * hostIP, int port)
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, hostIP, (void*)&serverAddr.sin_addr.s_addr);

	if (connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr) != 0))
	{
		return false;
	}

	m_isConnected = true;

	NonBlock(&m_socket);
	
	m_thread = thread([&]() { Update(); });

	return true;
}

bool TCPNetwork::IsConnected()
{
	return m_isConnected;
}

void TCPNetwork::DisConnect()
{
	if (m_isConnected)
	{
		closesocket(m_socket);

		Clear();
	}

	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void TCPNetwork::SendPacket(const short packetId, const short dataSize, char* pData)
{
	char data[MAX_PACKET_SIZE] = { 0, };

	PacketHeader packetHeader{ packetId, dataSize };
	memcpy(&data[0], (char*)&packetHeader, PACKET_HEADER_SIZE);

	if (dataSize > 0)
	{
		memcpy(&data[PACKET_HEADER_SIZE], pData, dataSize);
	}

	send(m_socket, data, dataSize + PACKET_HEADER_SIZE, 0);
}

void TCPNetwork::Update()
{
	while (m_isConnected)
	{
		RecvDate();
		RecvBufferProcess();
	}
}

RecvPacketInfo TCPNetwork::GetPacket()
{
	lock_guard<mutex> guard(m_mutex);

	if (m_packetQueue.empty())
	{
		return RecvPacketInfo();
	}

	auto packet = m_packetQueue.front();
	m_packetQueue.pop_front();
	return packet;
}

void TCPNetwork::NonBlock(SOCKET* s)
{
	u_long u10n = 1L;
	ioctlsocket(*s, FIONBIO, (unsigned long*)&u10n);
}

void TCPNetwork::RecvDate()
{
	fd_set read_set;
	timeval timeVal{ 0, 100 };

	FD_ZERO(&read_set);
	FD_SET(m_socket, &read_set);
	
	if (select(m_socket + 1, &read_set, NULL, NULL, &timeVal) < 0)
	{
		return;
	}

	if (FD_ISSET(m_socket, &read_set))
	{
		char recvBuf[MAX_PACKET_SIZE] = { 0, };

		auto recvSize = recv(m_socket, recvBuf, MAX_PACKET_SIZE, 0);

		if (recvSize == 0)
		{
			return;
		}

		if (recvSize < 0)
		{
			return;
		}

		if ((m_totalRecvSize + recvSize) >= MAX_SOCK_RECV_BUFFER)
		{
			return;
		}

		memcpy(&m_recvBuffer[m_totalRecvSize], recvBuf, recvSize);

		m_totalRecvSize += recvSize;
	}
}

void TCPNetwork::RecvBufferProcess()
{
	auto readPos = 0;
	const auto dataSize = m_totalRecvSize;
	PacketHeader* pPacketHeader;

	while ((dataSize - readPos) > PACKET_HEADER_SIZE)
	{
		pPacketHeader = (PacketHeader*)&m_recvBuffer[readPos];
		readPos += PACKET_HEADER_SIZE;

		if (pPacketHeader->bodySize > (dataSize - readPos))
		{
			break;
		}

		if (pPacketHeader->bodySize > MAX_PACKET_SIZE)
		{
			return;
		}

		AddPacketQueue(pPacketHeader->id, pPacketHeader->bodySize, &m_recvBuffer[readPos]);

		readPos += pPacketHeader->bodySize;
	}
}

void TCPNetwork::AddPacketQueue(const short packetId, const short bodySize, char* pDataPos)
{
	RecvPacketInfo recvPacketInfo;
	recvPacketInfo.packetId = packetId;
	recvPacketInfo.packetBodySize = bodySize;
	recvPacketInfo.pData = new char[bodySize];

	memcpy(recvPacketInfo.pData, pDataPos, bodySize);

	lock_guard<mutex> guard(m_mutex);
	m_packetQueue.push_back(recvPacketInfo);
}

void TCPNetwork::Clear()
{
	m_isConnected = false;
	m_totalRecvSize = 0;
	m_packetQueue.clear();
}
