#pragma once

#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <memory>
#include <thread>
#include <mutex>

#include <deque>

using namespace std;

#include "ErrorCode.h"
#include "PacketID.h"
#include "Packet.h"

const int MAX_PACKET_SIZE = 1024;
const int MAX_SOCK_RECV_BUFFER = 8016;

#pragma pack(push, 1)
struct PacketHeader
{
	short id;
	short bodySize;
};
#pragma pack(pop)

const int PACKET_HEADER_SIZE = sizeof(PacketHeader);

struct RecvPacketInfo
{
	RecvPacketInfo() = default;

	short packetId = 0;
	short packetBodySize = 0;
	char* pData = nullptr;
};

class TCPNetwork
{
public:
	TCPNetwork();
	~TCPNetwork();

	bool ConnectTo(const char* hostIP, int port);

	bool IsConnected();
	void DisConnect();

	void SendPacket(const short packetId, const short dataSize, char* pData);

	void Update();

	RecvPacketInfo GetPacket();

private:
	void NonBlock(SOCKET* s);
	
	void RecvDate();
	void RecvBufferProcess();

	void AddPacketQueue(const short packetId, const short bodySize, char* pDataPos);

	void Clear();

private:
	bool m_isConnected = false;

	thread m_thread;
	mutex m_mutex;

	SOCKET m_socket;

	int m_totalRecvSize = 0;
	char m_recvBuffer[MAX_SOCK_RECV_BUFFER] = { 0, };

	deque<RecvPacketInfo> m_packetQueue;
};