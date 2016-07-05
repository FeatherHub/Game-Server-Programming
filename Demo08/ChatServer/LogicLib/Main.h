#pragma once

#include <memory>

#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
/*
#include "../ServerNetLib/ILog.h"
#include "../ServerNetLib/TcpNetwork.h"

#include "LobbyManager.h"

#include "UserManager.h"

#include "PacketProcess.h"
*/
using ERROR_CODE = NCommon::ERROR_CODE;

namespace NServerNetLib
{
	struct ServerConfig;
	class ILog;
	class ITcpNetwork;
}

namespace NLogicLib
{
	class UserManager;
	class LobbyManager;
	class PacketProcess;

	class Main
	{
	public:
		Main() {}

		~Main() {}

		ERROR_CODE Init();

		void Run();

		void Stop();

	private:
		ERROR_CODE LoadConfig();
					
	private:
		bool m_IsRun = false;
		
		//init data
		std::unique_ptr<NServerNetLib::ServerConfig> m_pServerConfig;
		//logger
		std::unique_ptr<NServerNetLib::ILog> m_pLogger;
		//network handler
		std::unique_ptr<NServerNetLib::ITcpNetwork> m_pNetwork;
		//packet processor
		std::unique_ptr<PacketProcess> m_pPacketProc;
		//managers
		std::unique_ptr<UserManager> m_pUserMgr;
		std::unique_ptr<LobbyManager> m_pLobbyMgr;
	};
}
