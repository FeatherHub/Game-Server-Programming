#include "PktProcHeaders.h"

ERRORCODE PacketProcessor::LoginReq(char* pData, int clientId)
{
	LoginReqPkt* reqPkt = (LoginReqPkt*)pData;

	m_pUserManager->AddUser(reqPkt->id, clientId);

	LoginResPkt resPkt;
	resPkt.isPermiited = true;

	m_pRefNetwork->SendPacket(clientId, PacketId::LoginRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}