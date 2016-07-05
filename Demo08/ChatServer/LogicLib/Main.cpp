#include <thread>
#include <chrono>

#include "../ServerNetLib/ServerNetErrorCode.h"
#include "../ServerNetLib/Define.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "ConsoleLogger.h"
#include "LobbyManager.h"
#include "PacketProcess.h"
#include "UserManager.h"
#include "Main.h"




using LOG_TYPE = NServerNetLib::LOG_TYPE;
using NET_ERROR_CODE = NServerNetLib::NET_ERROR_CODE;

namespace NLogicLib
{																		
	ERROR_CODE Main::Init()
	{
		m_pLogger = std::make_unique<ConsoleLog>();

		LoadConfig();

		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
		m_pNetwork->Init(m_pServerConfig.get(), m_pLogger.get());

		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(m_pServerConfig->MaxClientCount);

		m_pLobbyMgr = std::make_unique<LobbyManager>();
		m_pLobbyMgr->Init({ m_pServerConfig->MaxLobbyCount, 
							m_pServerConfig->MaxLobbyUserCount,
							m_pServerConfig->MaxRoomCountByLobby, 
							m_pServerConfig->MaxRoomUserCount },
						m_pNetwork.get(), m_pLogger.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(), m_pLobbyMgr.get(), m_pLogger.get());

		m_IsRun = true;

		m_pLogger->Write(LOG_TYPE::L_INFO, "%s | Init Success. Client Run", __FUNCTION__);
		
		return ERROR_CODE::NONE;
	}

	void Main::Stop()
	{
		m_IsRun = false;
	}

	void Main::Run()
	{
		while (m_IsRun)
		{
			m_pNetwork->Run();

			while (true)
			{
				auto packetInfo = m_pNetwork->GetPacketInfo();

				if (packetInfo.PacketId == 0) break;
				
				m_pPacketProc->Process(packetInfo);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(0));
		}
	}

	ERROR_CODE Main::LoadConfig()
	{
		m_pServerConfig = std::make_unique<NServerNetLib::ServerConfig>();
		
		char sPath[MAX_PATH] = { 0, };
		GetCurrentDirectory(MAX_PATH, sPath);
		
		char inipath[MAX_PATH] = { 0, };
		_snprintf_s(inipath, _countof(inipath), _TRUNCATE, "%s\\ServerConfig.ini", sPath);

		m_pServerConfig->Port = (unsigned short)GetPrivateProfileInt("Config", "Port", 0, inipath);
		m_pServerConfig->BackLogCount = GetPrivateProfileInt("Config", "BackLogCount", 0, inipath);
		m_pServerConfig->MaxClientCount = GetPrivateProfileInt("Config", "MaxClientCount", 0, inipath);

		m_pServerConfig->MaxClientSockOptRecvBufferSize = (short)GetPrivateProfileInt("Config", "MaxClientSockOptRecvBufferSize", 0, inipath);
		m_pServerConfig->MaxClientSockOptSendBufferSize = (short)GetPrivateProfileInt("Config", "MaxClientSockOptSendBufferSize", 0, inipath);
		m_pServerConfig->MaxClientRecvBufferSize = (short)GetPrivateProfileInt("Config", "MaxClientRecvBufferSize", 0, inipath);
		m_pServerConfig->MaxClientSendBufferSize = (short)GetPrivateProfileInt("Config", "MaxClientSendBufferSize", 0, inipath);

		m_pServerConfig->ExtraClientCount = GetPrivateProfileInt("Config", "ExtraClientCount", 0, inipath);
		m_pServerConfig->MaxLobbyCount = GetPrivateProfileInt("Config", "MaxLobbyCount", 0, inipath);
		m_pServerConfig->MaxLobbyUserCount = GetPrivateProfileInt("Config", "MaxLobbyUserCount", 0, inipath);
		m_pServerConfig->MaxRoomCountByLobby = GetPrivateProfileInt("Config", "MaxRoomCountByLobby", 0, inipath);
		m_pServerConfig->MaxRoomUserCount = GetPrivateProfileInt("Config", "MaxRoomUserCount", 0, inipath);

		m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | Port(%d), Backlog(%d)", __FUNCTION__, m_pServerConfig->Port, m_pServerConfig->BackLogCount);
	
		return ERROR_CODE::NONE;
	}
}