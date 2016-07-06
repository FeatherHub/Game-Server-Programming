#pragma once

#include <memory>
//Packet 구조체 정보에 PacketId 정보까지 포함되어 있다
#include "../../Common/Packet.h"

//서버에서 사용하는 정보가 포함되어 있다
//Constants, ClientSession, 
//RecvPacketInfo, Packet Id, Packet header size
#include "../ServerNetLib/Define.h"

//서버와 클라이언트가 공유하는 에러코드가 있다.
//에러코드는 오류상황에 매칭된 코드이다.
#include "../../Common/ErrorCode.h"

using ERROR_CODE = NCommon::ERROR_CODE;

namespace NServerNetLib
{
	class ITcpNetwork;
	class ILog;
}

namespace NLogicLib
{	
	class ConnectedUserManager;
	class LobbyManager;
	class UserManager;

	#define CHECK_START  ERROR_CODE __result=ERROR_CODE::NONE;
	#define CHECK_ERROR(f) __result=f; goto PROCESS_ERROR;

	class PacketProcess
	{
		using TcpNet = NServerNetLib::ITcpNetwork;
		using ILog = NServerNetLib::ILog;

		using PacketInfo = NServerNetLib::RecvPacketInfo;
		//통신 상황에서 발생한 에러코드를 반환하고
		//사용할 데이터로 바꿀 패킷을 전달받는다
		//어떤 데이터인지는 호출시점에서 결정되어 있다. 
		typedef ERROR_CODE(PacketProcess::*PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

	public:
		PacketProcess();
		~PacketProcess();

		//멤버를 세팅하고
		//특히 Packet처리함수 배열을 매칭시킨다.
		void Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ILog* pLogger);

		//Packet처리함수 배열의 Packet id 위치를 호출한다.
		void Process(PacketInfo packetInfo);

		//로직 데이터들의 상태를 검사한다.
		void StateCheck();
	
	private:
		ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;
		UserManager* m_pRefUserMgr;
		LobbyManager* m_pRefLobbyMgr;
		std::unique_ptr<ConnectedUserManager> m_pConnectedUserManager;
						
	private:
		ERROR_CODE NtfSysConnctSession(PacketInfo packetInfo);
		
		ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);
		
		ERROR_CODE Login(PacketInfo packetInfo);
		
		ERROR_CODE LobbyList(PacketInfo packetInfo);

		ERROR_CODE LobbyEnter(PacketInfo packetInfo);

		ERROR_CODE LobbyRoomList(PacketInfo packetInfo);

		ERROR_CODE LobbyUserList(PacketInfo packetInfo);

		ERROR_CODE LobbyLeave(PacketInfo packetInfo);

		ERROR_CODE LobbyChat(PacketInfo packetInfo);

		//Packet Diagram
		//Requester	: ToWhom(Id), Msg
		//ToWhom	: Requester Id, Msg
		//Requester	: Whisper Success/Fail

		//Packet structure
		//PacketChatReq -> PacketWhisperReq
		//				[userId, msg]
		//PacketChatRes -> PacketWhisperRes
		//				[errorCode]
		//PacketChatNtf
		//				[RequesterId, msg]
		//Process Whisper
		//Chat -> NotifyAllUser: NotifyTargetUser
		ERROR_CODE LobbyWhisper(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);

		ERROR_CODE RoomChat(PacketInfo packetInfo);
	};
}