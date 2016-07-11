#include "../../Common/Packet.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "User.h"
#include "UserManager.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

//로비 입장 -> 해당 호비의 룸 리스트 & 유저 리스트
//로비 채팅 & 귓속말
//로비 퇴장

namespace NLogicLib
{
	//QSF
	ERROR_CODE PacketProcess::LobbyEnter(PacketInfo packetInfo)
	{
	CHECK_START
		//패킷을 읽는다.
		auto reqPkt = (NCommon::PktLobbyEnterReq*)packetInfo.pRefData;

		//데이터 & 논리의 유효성 검사
		//현재 위치가 로그인이 아니면 유효하지 않다
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLogIn() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(reqPkt->LobbyId);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_LOBBY_INDEX);
		}
		//검사 끝

		//요청클라의 요청을 처리한다.
		auto enterRet = pLobby->EnterUser(pUser);
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}
		
		//다른클라에게 결과를 통지한다.
		pLobby->NotifyLobbyEnterUserInfo(pUser);

		//요청클라에게 결과를 전송한다.
		NCommon::PktLobbyEnterRes resPkt;
		resPkt.MaxUserCount = pLobby->MaxUserCount();
		resPkt.MaxRoomCount = pLobby->MaxRoomCount();
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(NCommon::PktLobbyEnterRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	//해당 로비의 룸리스트를 다 보낼 때까지 계속 보낸다.(패킷다이어그램 참고)
	ERROR_CODE PacketProcess::LobbyRoomList(PacketInfo packetInfo)
	{
	CHECK_START
		//데이터와 논리의 유효성 검사
		//현재 위치가 로비가 아니면 유효하지 않다.
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_LOBBY_INDEX);
		}
		//검사 끝

		//요청클라의 요청을 처리한다.
		auto reqPkt = (NCommon::PktLobbyRoomListReq*)packetInfo.pRefData;
		//요청클라가 위치한 로비의 방 리스트를 전송한다.
		pLobby->SendRoomList(pUser->GetSessioIndex(), reqPkt->StartRoomIndex);
		//											(로비에서 처음 요청할 때는 0)
		return ERROR_CODE::NONE;

	PROCESS_ERROR :
		NCommon::PktLobbyRoomListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES, sizeof(NCommon::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	//QS
	ERROR_CODE PacketProcess::LobbyUserList(PacketInfo packetInfo)
	{
	CHECK_START
		//필요한 데이터 로드 & 데이터/로직의 유효성 검사
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
		//검사 끝

		//해당클라에게 유저리스트를 전송한다
		auto reqPkt = (NCommon::PktLobbyUserListReq*)packetInfo.pRefData;
		pLobby->SendUserList(pUser->GetSessioIndex(), reqPkt->StartUserIndex);

		return ERROR_CODE::NONE;
	PROCESS_ERROR:
		NCommon::PktLobbyUserListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, sizeof(NCommon::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
	
	//QSF
	ERROR_CODE PacketProcess::LobbyLeave(PacketInfo packetInfo)
	{
	CHECK_START
		//검사
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_LOBBY_INDEX);
		}
		//검사 끝

		//요청클라의 로비나가기 요청을 처리한다.
		auto enterRet = pLobby->LeaveUser(pUser->GetIndex());
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}

		//(요청클라는 퇴장처리됨)

		//다른클라들에게 갱신된 로비정보를 전송한다.
		pLobby->NotifyLobbyLeaveUserInfo(pUser);

		//요청클라에게 결과를 전송한다.
		NCommon::PktLobbyLeaveRes resPkt;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);

		return ERROR_CODE::NONE;
	PROCESS_ERROR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(NCommon::PktLobbyLeaveRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	//QSF
	ERROR_CODE PacketProcess::LobbyChat(PacketInfo packetInfo)
	{
	CHECK_START
		//로직에 필요한 데이터 로드
		//로직과 데이터의 유효성 검사
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_CHAT_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_CHAT_INVALID_LOBBY_INDEX);
		}
		//로드&검사 끝

		//요청클라가 요청한 서비스를 처리한다.

		//모든클라에게 서비스를 적용한다.
		//모든클라에게 요청클라의 아이디와 내용을 전송한다.
		auto reqPkt = (NCommon::PktLobbyChatReq*)packetInfo.pRefData;
		pLobby->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);
		
		//요청클라에게 성공/실패결과를 전송한다.
		NCommon::PktLobbyChatRes resPkt;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		
		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	//QSF
	//요청한 데이터 == 보낼 데이터 
	ERROR_CODE PacketProcess::LobbyWhisper(PacketInfo packetInfo)
	{
	CHECK_START
		//로직에 필요한 데이터를 로드한다
		//로직과 데이터의 유효성을 검사한다
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLobby() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_CHAT_INVALID_DOMAIN);
		}

		auto reqPkt = (NCommon::PktLobbyWhisperReq*)packetInfo.pRefData;
	
		auto pTargetRef = m_pRefUserMgr->GetUser(reqPkt->UserID);

		auto pTargetUser = std::get<1>(pTargetRef);

		auto& pTargetId = pTargetUser->GetID();
		//로드&검사 끝

		//Notify to target user
		m_pRefNetwork->SendData(pTargetUser->GetSessioIndex(), (short)packetInfo.PacketId, sizeof(reqPkt), (char*)&reqPkt);

		//Send result to requesting user
		m_pRefNetwork->SendData(pUser->GetSessioIndex(), (short)packetInfo.PacketId, sizeof(reqPkt), (char*)&reqPkt);
		
		NCommon::PktLobbyChatRes resPkt;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
}