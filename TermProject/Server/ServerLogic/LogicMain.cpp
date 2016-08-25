#include "LogicMain.h"
#include "..\Network\SelectNetwork.h"
#include "PacketManager.h"

#include "..\..\Common\Util\Logger.h"

void LogicMain::Init()
{
	m_isRun = true;

	m_network = new Network();
	auto res = m_network->Init(23452, "127.0.0.1");
	if (res == false)
	{
		Logger::Write(Logger::ERR, "Server:: Network init fail");
		return;
	}

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

			m_pktMgr->ProcessPacket(pkt);
		}
	}
}
