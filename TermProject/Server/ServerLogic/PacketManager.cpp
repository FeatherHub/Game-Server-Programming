#include "PacketManager.h"
#include "..\Network\SelectNetwork.h"

#include "..\..\Common\Util\Logger.h"

void PacketManager::Init(Network* network)
{
	m_pRefNetwork = network;
	m_pktProcessor.Init(network);

	Logger::Write(Logger::INFO, "Packet Processor init success");

	for (int i = 0; i < PacketId::MAX; i++)
	{
		m_pktFuncArray[i] = nullptr;
	}

	m_pktFuncArray[PacketId::TestReq] = &PacketProcessor::TestReq;
	m_pktFuncArray[PacketId::LoginReq] = &PacketProcessor::LoginReq;
	m_pktFuncArray[PacketId::LobbyUserNameListReq] = &PacketProcessor::LobbyUserListReq;

	Logger::Write(Logger::INFO, "Packet Func Array init success");
}

void PacketManager::ProcessPacket(RecvPacket& recvPkt)
{
	int pktId = recvPkt.id;

	if (pktId < 0 || pktId > PacketId::MAX)
	{
		m_pRefNetwork->BanClient(recvPkt.clientId);

		return;
	}

	if (m_pktFuncArray[pktId] == nullptr)
	{
		m_pRefNetwork->BanClient(recvPkt.clientId);

		return;
	}

	(m_pktProcessor.*m_pktFuncArray[pktId])(recvPkt.pDataAtBuff, recvPkt.clientId);
}