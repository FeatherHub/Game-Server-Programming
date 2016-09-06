#include "PktProcHeaders.h"

ERRORCODE PacketProcessor::LoginReq(char* pData, int clientIdx)
{
	//Get data from pkt
	LoginReqPkt* reqPkt = (LoginReqPkt*)pData;

	//Process server data
	m_pRefUserManager->AddUser(reqPkt->name, clientIdx);

	//Make response pkt
	LoginResPkt resPkt;
	resPkt.isPermiited = true;

	//Send resPkt
	m_pRefNetwork->SendPacket(clientIdx, (unsigned short)PacketId::LoginRes, (char*)&resPkt);

	//If permitted, Notify other clients
	if (resPkt.isPermiited == true)
	{
		m_pRefUserManager->NotifyNewbieLogin(clientIdx, reqPkt->name);
	}

	return ERRORCODE::NONE;
}