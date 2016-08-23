#include "Network.h"
#include "NetCode.h"

#include "..\..\Common\Util\BodySizeMananger.h"

Network* Network::m_instance = nullptr;

Network* Network::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new Network();
	}

	return m_instance;
}

Network::Network()
: m_readPos(0), m_recvSize(0), m_sendSize(0), m_sentSize(0), m_isConnected(false)
{
	Init();
}

bool Network::Init()
{
	int res;

	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0) return false;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) return false;

//	unsigned long mode = 1;	//non-blocking socket
//	res = ioctlsocket(m_socket, FIONBIO, &mode);
//	if (res != 0) return false;

	m_bodySizeMgr = new BodySizeManager();

	return true;
}

bool Network::ConnectTo(const char* ip, unsigned short port)
{
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, (PVOID)&serverAddr.sin_addr.s_addr);
	
	int res = connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (res == SOCKET_ERROR)
	{
//		if (WSAGetLastError() != WSAEWOULDBLOCK)

		return false;
	}

	m_isConnected = true;

	FD_ZERO(&m_readFd);
	FD_ZERO(&m_writeFd);

	FD_SET(m_socket, &m_readFd);

	return true;
}

NETCODE Network::SendPacket(unsigned short pktId, char* pData)
{
	int bodySize = m_bodySizeMgr->Get(pktId);

	if (m_sendSize + PACKET_HEADER_SIZE + bodySize > MAX_BUFF_SIZE)
	{
		return NETCODE::SEND_BUFFER_FULL;
	}

	//Copy pkt header
	CopyMemory(m_sendBuff + m_sendSize, &pktId, PACKET_HEADER_SIZE);

	//Copy body data right back
	CopyMemory(m_sendBuff + m_sendSize + PACKET_HEADER_SIZE, pData, bodySize);

	//Update member
	m_sendSize += PACKET_HEADER_SIZE + bodySize;

	return NETCODE::NONE;
}

void Network::Run()
{
	if (m_isConnected == false)
	{
		return;
	}

	NETCODE res = Select();
	if (res == NETCODE::SELECT_NO_CHANGE)
	{
		return;
	}

	if (FD_ISSET(m_socket, &m_readFd))
	{
		res = Recv();
		if (res == NETCODE::RECV_CONNECTION_CLOSED)
		{
			CloseConnect();

			return;
		}

		RecvBuffProc();
	}

	if (FD_ISSET(m_socket, &m_writeFd))
	{
		Send();

		SendBuffProc();
	}
}

NETCODE Network::Select()
{
	FD_ZERO(&m_writeFd);
	if (m_sendSize > 0)
	{
		FD_SET(m_socket, &m_writeFd);
	}

	FD_SET(m_socket, &m_readFd);

	timeval tv{ SELECT_WAIT_SEC, SELECT_WAIT_MILLSEC };

	int res = select(0, &m_readFd, &m_writeFd, nullptr, &tv);
	if (res == 0)
	{
		return NETCODE::SELECT_NO_CHANGE;
	}
	else if (res == SOCKET_ERROR)
	{
		return NETCODE::SELECT_SOCKET_ERROR;
	}

	return NETCODE::NONE;
}

NETCODE Network::Recv()
{
	CopyMemory(m_recvBuff, m_recvBuff + m_readPos, m_recvSize);

	int res = recv(m_socket, m_recvBuff + m_recvSize, MAX_BUFF_SIZE - m_recvSize, 0);
	if (res == 0)
	{
		return NETCODE::RECV_CONNECTION_CLOSED;
	}
	else if (res == SOCKET_ERROR)
	{
		return NETCODE::RECV_SOCKET_ERROR;
	}

	m_recvSize += res;

	return NETCODE::NONE;
}

//Cut byte stream into packet
void Network::RecvBuffProc()
{
	while (true)
	{
		if (m_recvSize < PACKET_HEADER_SIZE)
		{
			break;
		}

		PktHeader* pktH = (PktHeader*)(m_recvBuff + m_readPos);
		int bodySize = m_bodySizeMgr->Get(pktH->id);

		if (m_recvSize < PACKET_HEADER_SIZE + bodySize)
		{
			break;
		}
		
		char* dataPos = m_recvBuff + m_readPos + PACKET_HEADER_SIZE;

		AddToPktQueue(Packet{pktH->id, dataPos});

		m_recvSize -= PACKET_HEADER_SIZE + bodySize;
		m_readPos += PACKET_HEADER_SIZE + bodySize;
	}
}

NETCODE Network::Send()
{
	m_sentSize = 0;

	int res = send(m_socket, m_sendBuff, m_sendSize, 0);
	if (res == SOCKET_ERROR)
	{
		return NETCODE::SEND_SOCKET_ERROR;
	}

	m_sentSize = res;

	return NETCODE::NONE;
}

void Network::SendBuffProc()
{
	//update member
	m_sendSize -= m_sentSize;

	//pull rest forward
	CopyMemory(m_sendBuff, m_sendBuff + m_sentSize, m_sendSize);
}

void Network::AddToPktQueue(Packet&& pkt)
{
	m_pktQueue.emplace(pkt);
}

Packet Network::GetPacket()
{
	Packet pkt = m_pktQueue.back();
	m_pktQueue.pop();

	return pkt;
}

void Network::CloseConnect()
{
	closesocket(m_socket);
	m_isConnected = false;	

	FD_ZERO(&m_readFd);
	FD_ZERO(&m_writeFd);

	while (m_pktQueue.empty() == false) 
		m_pktQueue.pop();

	m_recvSize = 0;
	m_readPos = 0;
	m_sendSize = 0;
	m_sentSize = 0;
}

void Network::DisplayErrorMsg(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL);

	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

