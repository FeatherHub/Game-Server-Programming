#include "PktProcHeaders.h"

void PacketProcessor::Init(Network* network, UserManager* userMgr)
{
	m_pRefNetwork = network;
	m_pRefUserManager = userMgr;
}

ERRORCODE PacketProcessor::TestReq(char* pData, int clientIdx)
{
	//데이터 추출
	TestReqPkt* reqPkt = (TestReqPkt*)pData;
	int cnt = reqPkt->num;

	//데이터 포장
	TestResPkt resPkt;
	resPkt.num = cnt;

	//데이터 전송
	m_pRefNetwork->SendPacket(clientIdx, (unsigned short)PacketId::TestRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}
