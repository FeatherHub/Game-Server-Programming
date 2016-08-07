#include "PacketManager.h"
#include "PacketProcessor.h"

void PacketManager::Init()
{
	for (int i = 0; i < PacketId::MAX; i++)
	{
		m_pktFuncArray[i] = nullptr;
	}

	m_pktFuncArray[PacketId::LoginReq] = &PacketProcessor::LoginReq;
}

void PacketManager::ProcessPacket(RecvPacket recvPkt)
{
	int pktId = recvPkt.id;

	if (pktId < 0 || pktId > PacketId::MAX)
	{
		return;
	}


}

