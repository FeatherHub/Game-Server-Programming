#include "PktProcHeaders.h"

void PacketProcessor::Init(Network* network)
{
	m_pRefNetwork = network;

	m_pUserManager = new UserManager();
	m_pUserManager->Init();
}

ERRORCODE PacketProcessor::TestReq(char* pData, int clientId)
{
	//데이터 추출
	TestReqPkt* reqPkt = (TestReqPkt*)pData;
	int cnt = reqPkt->num;

	//데이터 포장
	TestResPkt resPkt;
	resPkt.num = cnt;

	//데이터 전송
	m_pRefNetwork->SendPacket(clientId, PacketId::TestRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}
