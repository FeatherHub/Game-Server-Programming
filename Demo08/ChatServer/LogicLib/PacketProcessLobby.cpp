#include "../../Common/Packet.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "User.h"
#include "UserManager.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

/*흐름*/
//전달받은 패킷 가공하기

//가공한 패킷에서 정보 얻기
//매니저를 통해 정보 얻기
//번호로 정보 얻기

//유효한 정보인지 검사하기

//실질적인 작업 처리하기

//작업결과 전송하기

namespace NLogicLib
{
	ERROR_CODE PacketProcess::LobbyEnter(PacketInfo packetInfo)
	{
	CHECK_START
		// 현재 위치 상태는 로그인이 맞나?
		// 로비에 들어간다.
		// 기존 로비에 있는 사람에게 새 사람이 들어왔다고 알려준다

		//전달받은 패킷을 로비엔터패킷으로 가공한다
		auto reqPkt = (NCommon::PktLobbyEnterReq*)packetInfo.pRefData;

		//로비엔터에서의 결과를 저장할 변수를 선언한다
		NCommon::PktLobbyEnterRes resPkt;

		//전달받은 패킷에서 세션번호 정보를 꺼내 유저매니저를 통해 유저 정보를 얻는다 
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		//유저 정보에서 결과 저장 변수를 얻는다
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) 
		{
			CHECK_ERROR(errorCode);
		}

		//유저 정보에서 유저 저장 변수를 얻는다
		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLogIn() == false) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_DOMAIN);
		}

		//가공한 패킷에서 로비 아이디를 얻는다.
		//로비매니저로 로비 정보를 얻는다.
		auto pLobby = m_pRefLobbyMgr->GetLobby(reqPkt->LobbyId);
		if (pLobby == nullptr) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_LOBBY_INDEX);
		}

		//로비 정보에 유저 정보를 전달하여 엔터유저를 호출한다
		auto enterRet = pLobby->EnterUser(pUser);
		if (enterRet != ERROR_CODE::NONE) 
		{
			CHECK_ERROR(enterRet);
		}
		
		//로비 정보에 유저 정보를 전달하여 로티로비엔터유저를 호출한다
		pLobby->NotifyLobbyEnterUserInfo(pUser);

		resPkt.MaxUserCount = pLobby->MaxUserCount();
		resPkt.MaxRoomCount = pLobby->MaxRoomCount();

		m_pRefNetwork->SendData(packetInfo.SessionIndex, 
								(short)PACKET_ID::LOBBY_ENTER_RES, 
								sizeof(NCommon::PktLobbyEnterRes), 
								(char*)&resPkt);
		
		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, 
								(short)PACKET_ID::LOBBY_ENTER_RES, 
								sizeof(NCommon::PktLobbyEnterRes), 
								(char*)&resPkt);

		return (ERROR_CODE)__result;
	}



	ERROR_CODE PacketProcess::LobbyRoomList(PacketInfo packetInfo)
	{
	CHECK_START
		// 현재 로비에 있는지 조사한다.
		// 룸 리스트를 보내준다.
		
		//전달받은 패킷에서 세션번호를 얻는다
		//유저매니저를 통해 세션번호로 유저정보를 얻는다
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) 
		{
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_DOMAIN);
		}

		//유저정보에서 로비번호를 얻는다
		//로비매니저를 통해 로비번호로 로비정보를 얻는다
		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_LOBBY_INDEX);
		}

		//전달받은 패킷의 바디를 로비룸리스트 패킷으로 가공한다
		auto reqPkt = (NCommon::PktLobbyRoomListReq*)packetInfo.pRefData;

		//로비룸리스트 패킷에서 시작룸번호를 얻는다
		//유저정보에서 세션번호를 얻는다
		//로비정보를 통해 룸리스트전송을 호출한다
		//세션정보와 시작룸번호를 전달한다.
		pLobby->SendRoomList(pUser->GetSessioIndex(), reqPkt->StartRoomIndex);

		return ERROR_CODE::NONE;

	CHECK_ERR :
		NCommon::PktLobbyRoomListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, 
								(short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES, 
								sizeof(NCommon::PktBase), 
								(char*)&resPkt);

		return (ERROR_CODE)__result;
	}



	ERROR_CODE PacketProcess::LobbyUserList(PacketInfo packetInfo)
	{
	CHECK_START
		// 현재 로비에 있는지 조사한다.
		// 유저 리스트를 보내준다.

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_USER_LIST_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_USER_LIST_INVALID_LOBBY_INDEX);
		}

		auto reqPkt = (NCommon::PktLobbyUserListReq*)packetInfo.pRefData;

		pLobby->SendUserList(pUser->GetSessioIndex(), reqPkt->StartUserIndex);

		return ERROR_CODE::NONE;

	CHECK_ERR:
		NCommon::PktLobbyUserListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, sizeof(NCommon::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
	


	ERROR_CODE PacketProcess::LobbyLeave(PacketInfo packetInfo)
	{
	CHECK_START
		// 현재 로비에 있는지 조사한다.
		// 로비에서 나간다
		// 기존 로비에 있는 사람에게 나가는 사람이 있다고 알려준다.

		//로비리브레스에서 사용할 결과 패킷을 선언한다
		NCommon::PktLobbyLeaveRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) 
		{
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) 
		{
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_LOBBY_INDEX);
		}

		auto enterRet = pLobby->LeaveUser(pUser->GetIndex());
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}

		pLobby->NotifyLobbyLeaveUserInfo(pUser);
				
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);

		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
}