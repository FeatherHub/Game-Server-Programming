#include "PacketProcessor.h"
#include "..\Network\SelectNetwork.h"

#include "..\..\Common\ErrorCode.h"
#include "..\..\Common\Packet.h"
#include "..\..\Common\Packetid.h"
#include "..\..\Common\Util\Logger.h"
#include "..\Network\NetPacket.h"

void PacketProcessor::Init(Network* network)
{
	m_pRefNetwork = network;
}

ERRORCODE PacketProcessor::TestReq(char* pData, int clientId)
{
	TestReqPkt* reqPkt = (TestReqPkt*)pData;
	int cnt = reqPkt->num;

	Logger::Write(Logger::INFO, "Get is %d", cnt);

	while (cnt > 0)
	{
		TestResPkt resPkt;
		resPkt.num = cnt;

		m_pRefNetwork->SendPacket(clientId, PacketId::TestRes, (char*)&resPkt);

		Logger::Write(Logger::INFO, "cnt %d", cnt);

		cnt--;
	}

	Logger::Write(Logger::INFO, "Done");

	return ERRORCODE::NONE;
}

ERRORCODE PacketProcessor::LoginReq(char* pData, int clientId)
{
	return ERRORCODE::NONE;
}

