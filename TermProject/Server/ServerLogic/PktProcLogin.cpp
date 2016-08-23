#include "PktProcHeaders.h"

ERRORCODE PacketProcessor::LoginReq(char* pData, int clientId)
{
	Logger::Write(Logger::Level::INFO, "LoginReq In");

	//Get data from pkt
	LoginReqPkt* reqPkt = (LoginReqPkt*)pData;

	//Process server data
	m_pUserManager->AddUser(reqPkt->id, clientId);

	//Make response pkt
	LoginResPkt resPkt;
	resPkt.isPermiited = true;

	//Send resPkt
	m_pRefNetwork->SendPacket(clientId, PacketId::LoginRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}