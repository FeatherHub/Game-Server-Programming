#include "BodySizeMananger.h"

#include "..\Packet.h"

void BodySizeManager::Init()
{
	m_bodySizeArr[PacketId::TestReq] = sizeof(TestReqPkt);
	m_bodySizeArr[PacketId::TestRes] = sizeof(TestResPkt);
	//m_bodySizeArr[PacketId::LoginReq] = sizeof(LoginReqPkt);
	//m_bodySizeArr[PacketId::LoginRes] = sizeof(LoginResPkt);
}

int BodySizeManager::Get(unsigned short pktId)
{
	if (pktId < 0 || pktId > PacketId::MAX)
	{
		return 0;
	}

	return m_bodySizeArr[pktId];
}
