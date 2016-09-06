#include "NetPacketQueue.h"
#include "NetData.h"
#include <Windows.h>

NetPacketQueue::RecvPacket NetPacketQueue::GetPacket()
{
	//m_mutex.lock();
	//
	//if (m_recvPktQueue.empty() == true)
	//{
	//	m_mutex.unlock();
	//	return RecvPacket();
	//}

	m_mutex.lock();

	auto recvPkt = m_recvPktQueue.front();
	m_recvPktQueue.pop();

	m_mutex.unlock();

	return recvPkt;
}

void NetPacketQueue::Push(RecvPacket& refPacket, int bodySize)
{
	RecvPacket packet;
	packet.clientIdx = refPacket.clientIdx;
	packet.id = refPacket.id;
	packet.pDataAtBuff = new char[bodySize];
	CopyMemory(packet.pDataAtBuff, refPacket.pDataAtBuff, bodySize);

	m_mutex.lock();
	m_recvPktQueue.push(packet);
	m_mutex.unlock();
}

bool NetPacketQueue::Empty()
{
	m_mutex.lock();
	auto res = m_recvPktQueue.empty();
	m_mutex.unlock();
	
	return res;
}