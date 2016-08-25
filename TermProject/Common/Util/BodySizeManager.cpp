#include "BodySizeMananger.h"

#include "..\Packet.h"

BodySizeManager::BodySizeManager()
{
	Init();
}

void BodySizeManager::Init()
{
	m_bodySizeArr[(int)PacketId::TestReq] = sizeof(TestReqPkt);
	m_bodySizeArr[(int)PacketId::TestRes] = sizeof(TestResPkt);

	m_bodySizeArr[(int)PacketId::LoginReq] = sizeof(LoginReqPkt);
	m_bodySizeArr[(int)PacketId::LoginRes] = sizeof(LoginResPkt);
	m_bodySizeArr[(int)PacketId::LoginNtf] = sizeof(LoginNtfPkt);
	
	m_bodySizeArr[(int)PacketId::LobbyUserNameListReq] = sizeof(LobbyUserNameListReqPkt);
	m_bodySizeArr[(int)PacketId::LobbyUserNameListRes] = sizeof(LobbyUserNameListResPkt);

	m_bodySizeArr[(int)PacketId::LobbyChatReq] = sizeof(LobbyChatReqPkt);
	m_bodySizeArr[(int)PacketId::LobbyChatRes] = sizeof(LobbyChatResPkt);
	m_bodySizeArr[(int)PacketId::LobbyChatNtf] = sizeof(LobbyChatNtfPkt);

	m_bodySizeArr[(int)PacketId::RemoveUserNtf] = sizeof(RemoveUserNtfPkt);
}

int BodySizeManager::Get(unsigned short pktId)
{
	if (pktId < 0 || pktId >(int)PacketId::MAX)
	{
		return 0;
	}

	return m_bodySizeArr[pktId];
}
