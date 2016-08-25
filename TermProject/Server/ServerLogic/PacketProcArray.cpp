#include "PacketProcArray.h"
#include "PacketProcessor.h"

#include "..\Network\SelectNetwork.h"

#include "..\..\Common\Util\Logger.h"

void PacketProcArray::Init(Network* network, UserManager* userMgr)
{
	m_pRefNetwork = network;

	m_pPktProcessor = new PacketProcessor();
	m_pPktProcessor->Init(network, userMgr);

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

	if (pktId < 0 || pktId >(int)PacketId::MAX ||
		m_pktFuncArray[pktId] == nullptr)
	{
		m_pRefNetwork->ForceCloseClient(recvPkt.clientId);
		
		Logger::Write(Logger::INFO, "Unavailable Pkt id (%d)", pktId);

		return;
	}

	Logger::Write(Logger::INFO, "Got packet id %d", pktId);

	(m_pPktProcessor->*m_pktFuncArray[pktId])(recvPkt.pDataAtBuff, recvPkt.clientId);
}
