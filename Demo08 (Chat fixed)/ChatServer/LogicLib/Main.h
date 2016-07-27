#pragma once
#include <memory>

//패킷 종류에 따른 구조체
//패킷은 기능에 따른 데이터를 가진다.
#include "../../Common/Packet.h"
//통신 오류 종류를 선언해놓은 열거형
//오류는 상황에 따른 코드를 가진다.
#include "../../Common/ErrorCode.h"

//네임스페이스 생략
using ERROR_CODE = NCommon::ERROR_CODE;

//통신 관련 전방 선언
namespace NServerNetLib
{
	struct ServerConfig;
	class ILog;
	class ITcpNetwork;
}

namespace NLogicLib
{
	//로직 관련 전방 선언
	class UserManager;
	class LobbyManager;
	class PacketProcess;

	class Main
	{
	public:
		//헤더 포함 안 되고 전방 선언된 자료구조가 있는 경우
		//생성자와 소멸자는 소스파일에서 구현되어야 한다
		//컴파일러가 멤버에 대해서 생성,소멸 코드를 넣기 때문에
		Main();
		~Main();

		//호출부에서 init -> run loop -> stop 순서로 세팅한다.
		ERROR_CODE Init();
		void Run();
		void Stop();

	private:
		//init sub
		ERROR_CODE LoadConfig();
		//~main sub
		void Release();

	private:
		//stop sub
		bool m_IsRun = false;

		std::unique_ptr<NServerNetLib::ITcpNetwork> m_pNetwork;
		std::unique_ptr<NServerNetLib::ServerConfig> m_pServerConfig;
		std::unique_ptr<NServerNetLib::ILog> m_pLogger;

		std::unique_ptr<PacketProcess> m_pPacketProc;
		std::unique_ptr<UserManager> m_pUserMgr;
		std::unique_ptr<LobbyManager> m_pLobbyMgr;
	};
}
