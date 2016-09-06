#pragma once

#include <mutex>
#include <queue>

#include "NetData.h"

class PacketQueue
{
public:
	RecvPacket Front();
	void Push(RecvPacket& refPacket);
	bool Empty();

private:
	std::queue<RecvPacket> m_recvPktQueue;
	std::mutex m_mutex;
};