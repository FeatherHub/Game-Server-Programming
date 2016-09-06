#include "LogicMain.h"
#include "..\Network\IOCPNetwork.h"
#include "PacketProcArray.h"
#include "UserManager.h"

#include "..\..\Common\Util\Logger.h"

void LogicMain::Init()
{
	m_isRun = true;

	m_pNetwork = new Network();
	auto res = m_pNetwork->Init(23452);
	if (res == NETCODE::SOCKET_FUNC_ERROR)
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
	m_pNetwork->Run();

	while (m_isRun)
	{
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
	auto& closedClientIdxPool = m_pNetwork->GetClosedClientQueue();
	auto& closedClientPoolMutex = m_pNetwork->GetClosedClientQueueMutex();

	closedClientPoolMutex.lock();
	while (closedClientIdxPool.empty() == false)
	{
		auto clientIdx = closedClientIdxPool.front();
		closedClientIdxPool.pop();
		closedClientPoolMutex.unlock();

		m_pUserMgr->RemoveUser(clientIdx);

		closedClientPoolMutex.lock();
	}
	closedClientPoolMutex.unlock();
}
