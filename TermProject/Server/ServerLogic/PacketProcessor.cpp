#include "PacketProcessor.h"
#include "SelectNetwork.h"

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
	Logger::Write(Logger::INFO, "TestReq");

	TestReqPkt* reqPkt = (TestReqPkt*)data;
	int cnt = reqPkt->num;

	while (cnt > 0)
	{
		TestResPkt resPkt;
		resPkt.num;

		m_pRefNetwork->SendPacket(clientId, Packet{PacketId::TestRes, (char*)&resPkt});

		cnt--;
	}

	return ERRORCODE::NONE;
}

ERRORCODE PacketProcessor::LoginReq(char* data, int clientId)
{
	return ERRORCODE::NONE;
}

