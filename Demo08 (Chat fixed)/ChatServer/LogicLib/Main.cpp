#include <thread>
#include <chrono>

//서버 오류 종류(User한테 갈 오류코드)를 선언해놓은 열거형
#include "../ServerNetLib/ServerNetErrorCode.h"
//서버에서 사용하는 구조체, 열거형을 모아놓은 파일
//ServerConfig, ClientSession, RecvPacket, PacketHeader
//Packet Id, Socket close case (User한테 가지 않을 오류코드)
#include "../ServerNetLib/Define.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "ConsoleLogger.h"
#include "LobbyManager.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "Main.h"

using LOG_TYPE = NServerNetLib::LOG_TYPE;
using NET_ERROR_CODE = NServerNetLib::NET_ERROR_CODE;

namespace NLogicLib
{
	Main::Main()
	{
	}

	Main::~Main()
	{
		Release();
	}

	ERROR_CODE Main::Init()
	{
		//Interface에 Implemented를 넣는다
		//Interface는 Mockup Implemented를 통해 Unit test를 하기 위해서다
		m_pLogger = std::make_unique<ConsoleLog>();

		//m_pServerConfig 초기화
		LoadConfig();

		//네트워크를 생성한다.
		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
		//네트워크를 초기화환다.
		//서버설정과 로거를 공유한다.
		auto result = m_pNetwork->Init(m_pServerConfig.get(), m_pLogger.get());
		
		//서버설정에 예외나 오류가 발생한 경우에 대해 처리한다
		if (result != NET_ERROR_CODE::NONE)
		{
			m_pLogger->Write(LOG_TYPE::L_ERROR, "%s | Init Fail. NetErrorCode(%s)", __FUNCTION__, (short)result);
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}
		
		//유저매니저를 생성한다.
		m_pUserMgr = std::make_unique<UserManager>();
		//유저매니저를 초기화한다.
		//서버설정으로부터 정보를 얻는다.
		m_pUserMgr->Init(m_pServerConfig->MaxClientCount);

		//로비매니저를 생성한다.
		m_pLobbyMgr = std::make_unique<LobbyManager>();
		//로비매니저를 초기화한다.
		//서버설정으로부터 정보를 얻는다.
		//네트워크와 로거를 공유한다.
		m_pLobbyMgr->Init({ m_pServerConfig->MaxLobbyCount, 
							m_pServerConfig->MaxLobbyUserCount,
							m_pServerConfig->MaxRoomCountByLobby, 
							m_pServerConfig->MaxRoomUserCount },
						m_pNetwork.get(), m_pLogger.get());

		//패킷프로세서를 생성한다
		m_pPacketProc = std::make_unique<PacketProcess>();
		//패킷프로세서를 초기화한다.
		//모든 정보를 공유한다.
		m_pPacketProc->Init(m_pNetwork.get(), 
							m_pUserMgr.get(), 
							m_pLobbyMgr.get(), 
							m_pLogger.get());

		//작동 여부를 초기화한다.
		m_IsRun = true;

		//Main-초기화 완료
		m_pLogger->Write(LOG_TYPE::L_INFO, "%s | Version2.0 Init Complete. Server Run", __FUNCTION__);
		
		return ERROR_CODE::NONE;
	}

	void Main::Run()
	{
		//구동 중인 동안
		while (m_IsRun)
		{
			//네트워크를 구동한다
			m_pNetwork->Run();

			//구동을 하며 받은 패킷과
			//처리를 통해 보내야할 패킷을
			//모두 처리할 때까지
			//패킷 읽기-처리 작업을 반복한다
			while (true)
			{
				//큐에서 패킷을 하나 읽어 온다.
				auto packetInfo = m_pNetwork->GetPacketInfo();

				//큐의 패킷을 모두 처리했다
				if (packetInfo.PacketId == 0)
				{
					break;
				}
				else
				{
					//읽어온 패킷을 처리한다.
					//1. packet id -> packet function call
					//2. packet id -> packet data to functional data
					//3. process by function and send(copy data to a session's buffer)
					//4. return result
					m_pPacketProc->Process(packetInfo);
				}
			}

			//데이터들에 대해 상태를 검사하고
			//결과에 따라 데이터를 처리한다.
			m_pPacketProc->StateCheck();
		}
	}

	void Main::Stop()
	{
		m_IsRun = false;
	}

	void Main::Release()
	{
		if (m_pNetwork)
			m_pNetwork->Release();
	}

	ERROR_CODE Main::LoadConfig()
	{
		m_pServerConfig = std::make_unique<NServerNetLib::ServerConfig>();
		
		wchar_t sPath[MAX_PATH] = { 0, };
		GetCurrentDirectory(MAX_PATH, sPath);

		wchar_t inipath[MAX_PATH] = { 0, };
		_snwprintf_s(inipath, _countof(inipath), _TRUNCATE, L"%s\\ServerConfig.ini", sPath);

		m_pServerConfig->Port = (unsigned short)GetPrivateProfileInt(L"Config", L"Port", 0, inipath);
		m_pServerConfig->BackLogCount = GetPrivateProfileInt(L"Config", L"BackLogCount", 0, inipath);
		m_pServerConfig->MaxClientCount = GetPrivateProfileInt(L"Config", L"MaxClientCount", 0, inipath);

		m_pServerConfig->MaxClientSockOptRecvBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSockOptRecvBufferSize", 0, inipath);
		m_pServerConfig->MaxClientSockOptSendBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSockOptSendBufferSize", 0, inipath);
		m_pServerConfig->MaxClientRecvBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientRecvBufferSize", 0, inipath);
		m_pServerConfig->MaxClientSendBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSendBufferSize", 0, inipath);

		m_pServerConfig->ExtraClientCount = GetPrivateProfileInt(L"Config", L"ExtraClientCount", 0, inipath);
		m_pServerConfig->MaxLobbyCount = GetPrivateProfileInt(L"Config", L"MaxLobbyCount", 0, inipath);
		m_pServerConfig->MaxLobbyUserCount = GetPrivateProfileInt(L"Config", L"MaxLobbyUserCount", 0, inipath);
		m_pServerConfig->MaxRoomCountByLobby = GetPrivateProfileInt(L"Config", L"MaxRoomCountByLobby", 0, inipath);
		m_pServerConfig->MaxRoomUserCount = GetPrivateProfileInt(L"Config", L"MaxRoomUserCount", 0, inipath);

		m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | Port(%d), Backlog(%d)", __FUNCTION__, m_pServerConfig->Port, m_pServerConfig->BackLogCount);
		
		return ERROR_CODE::NONE;
	}
}