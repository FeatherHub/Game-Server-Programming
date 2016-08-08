#include "LogicMain.h"
#include "SelectNetwork.h"
#include "PacketManager.h"

#include "..\..\Common\Util\Logger.h"

void LogicMain::Init()
{
	m_isRun = true;

	m_network = new Network();
	m_network->Init(23452, "127.0.0.1");

	m_pktMgr = new PacketManager();
	m_pktMgr->Init(m_network);
}

void LogicMain::Run()
{
	while (m_isRun)
	{
		m_network->Run();

		while (m_network->PacketQueueEmpty() == false)
		{
			auto pkt = m_network->GetPacket();

			Logger::Write(Logger::INFO, "Extract a packet %d", pkt.id);

			m_pktMgr->ProcessPacket(pkt);

			Logger::Write(Logger::INFO, "Process packet");
		}
	}
}