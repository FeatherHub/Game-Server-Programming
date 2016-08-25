#include "PacketProcArray.h"

void PacketProcArray::RegisterPktProcArr()
{
	m_pktFuncArray[(int)PacketId::TestReq] = &PacketProcessor::TestReq;
	m_pktFuncArray[(int)PacketId::LoginReq] = &PacketProcessor::LoginReq;
	m_pktFuncArray[(int)PacketId::LobbyUserNameListReq] = &PacketProcessor::LobbyUserListReq;
}