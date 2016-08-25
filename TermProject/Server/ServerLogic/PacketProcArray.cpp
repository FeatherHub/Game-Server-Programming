#include "PacketProcArray.h"
#include "..\Network\SelectNetwork.h"

#include "..\..\Common\Util\Logger.h"

void PacketProcArray::Init(Network* network)
{
	m_pRefNetwork = network;
	m_pktProcessor.Init(network);

	for (int i = 0; i < (int)PacketId::MAX; i++)
	{
		m_pktFuncArray[i] = nullptr;
	}

	RegisterPktProcArr();

	Logger::Write(Logger::INFO, "Packet Processor init success");
}

void PacketProcArray::ProcessPacket(RecvPacket& recvPkt)
{
	int pktId = recvPkt.id;

	if (pktId < 0 || pktId >(int)PacketId::MAX)
	{
		m_pRefNetwork->BanClient(recvPkt.clientId);
		
		Logger::Write(Logger::INFO, "Unavailable Pkt id (%d)", pktId);

		return;
	}

	if (m_pktFuncArray[pktId] == nullptr)
	{
		m_pRefNetwork->BanClient(recvPkt.clientId);
		
		Logger::Write(Logger::INFO, "Unavailable Pkt id (%d)", pktId);

		return;
	}

	Logger::Write(Logger::INFO, "Got packet id %d", pktId);

	(m_pktProcessor.*m_pktFuncArray[pktId])(recvPkt.pDataAtBuff, recvPkt.clientId);
}