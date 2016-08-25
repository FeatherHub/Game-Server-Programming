#include "LogicMain.h"
#include "..\Network\SelectNetwork.h"
#include "PacketProcArray.h"
#include "UserManager.h"

#include "..\..\Common\Util\Logger.h"

void LogicMain::Init()
{
	m_isRun = true;

	m_pNetwork = new Network();
	auto res = m_pNetwork->Init(23452, "127.0.0.1");
	if (res == false)
	{
		Logger::Write(Logger::ERR, "Server:: Network init fail");
		return;
	}

	m_pUserMgr = new UserManager();
	m_pUserMgr->Init(m_pNetwork);

	m_pktProcArr = new PacketProcArray();
	m_pktProcArr->Init(m_pNetwork, m_pUserMgr);
}

void LogicMain::Run()
{
	while (m_isRun)
	{
		m_pNetwork->Run();

		while (m_pNetwork->PacketQueueEmpty() == false)
		{
			auto pkt = m_pNetwork->GetPacket();

			m_pktProcArr->ProcessPacket(pkt);
		}

		PostProcess();
	}
}

void LogicMain::PostProcess()
{
	auto& clientIdxToClose = m_pNetwork->GetClosedClients();
	while (clientIdxToClose.empty() == false)
	{
		auto clientIdx = clientIdxToClose.front();
		clientIdxToClose.pop();

		m_pUserMgr->RemoveUser(clientIdx);
	}
}
