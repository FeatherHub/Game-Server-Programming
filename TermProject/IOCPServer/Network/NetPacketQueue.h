#pragma once

#include <mutex>
#include <queue>

#include "NetPacket.h"

class NetPacketQueue
{
	using RecvPacket = NetworkLib::NetPacket;
public:
	RecvPacket GetPacket();
	void Push(RecvPacket& refPacket, int bodySize);
	bool Empty();

private:
	std::queue<RecvPacket> m_recvPktQueue;
	std::mutex m_mutex;
};