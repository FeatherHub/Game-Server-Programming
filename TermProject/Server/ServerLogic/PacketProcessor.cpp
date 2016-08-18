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

ERRORCODE PacketProcessor::TestReq(char* data, int clientId)
{
	TestReqPkt* reqPkt = (TestReqPkt*)data;
	int cnt = reqPkt->num;

	Logger::Write(Logger::INFO, "Get is %d", cnt);

	while (cnt > 0)
	{
		TestResPkt resPkt;
		resPkt.num = cnt;

		m_pRefNetwork->SendPacket(clientId, Packet{PacketId::TestRes, (char*)&resPkt});

		cnt--;
	}

	return ERRORCODE::NONE;
}

ERRORCODE PacketProcessor::LoginReq(char* data, int clientId)
{
	return ERRORCODE::NONE;
}

