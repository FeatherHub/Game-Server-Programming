#include "PacketQueue.h"

#include <Windows.h>

RecvPacket PacketQueue::Front()
{
	m_mutex.lock();

	if (m_recvPktQueue.empty() == true)
	{
		m_mutex.unlock();
		return RecvPacket();
	}

	auto recvPkt = m_recvPktQueue.front();
	m_recvPktQueue.pop();

	m_mutex.unlock();

	return recvPkt;
}

void PacketQueue::Push(RecvPacket& refPacket)
{

	RecvPacket packet;
	packet.clientIdx = refPacket.clientIdx;
	packet.packetId = refPacket.packetId;
/*
	int bodySize = BodySizeManager->GetBodySize(packetId);
	packet.pData = new char[bodysize];
	CopyMemory(packet.pData, refPacket.pData, bodySize);
*/
	m_mutex.lock();

	m_recvPktQueue.push(refPacket);

	m_mutex.unlock();
}

bool PacketQueue::Empty()
{
	m_mutex.lock();
	if (m_recvPktQueue.empty() == true)
	{
		m_mutex.unlock();
		return true;
	}
	else
	{
		m_mutex.unlock();
		return false;
	}
}