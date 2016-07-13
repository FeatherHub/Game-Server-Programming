#include "../../Common/Packet.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "ConnectedUserManager.h"
#include "User.h"
#include "UserManager.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = NCommon::PACKET_ID;

//로그인 -> 로비 리스트

namespace NLogicLib
{
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
		//packetInfo는 client로부터 전송받은 Login과 관련된 packet을 가진다.
	{
	CHECK_START
		auto reqPkt = (NCommon::PktLogInReq*)packetInfo.pRefData;

		//로그인 요청한 유저를 유저풀에 추가한다.
		auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);

		if (addRet != ERROR_CODE::NONE) {
			CHECK_ERROR(addRet);
		}

		//로그인 요청한 유저의 상태를 변경한다.
		m_pConnectedUserManager->SetLogin(packetInfo.SessionIndex);

		//로그인 요청한 유저에게 처리결과를 전송한다.
		NCommon::PktLogInRes resPkt;
		resPkt.ErrorCode = (short)addRet;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);

		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::LobbyList(PacketInfo packetInfo)
	{
	CHECK_START
		//요청이 유효한지 검사한다.
		//현재 상태가 로그인이 아니면 유효하지 않다.
		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}
	
		auto pUser = std::get<1>(pUserRet);
		
		if (pUser->IsCurDomainInLogIn() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN);
		}
		//검사 끝

		//요쳥한 클라에게 로비리스트를 전송한다.
		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);
		
		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		NCommon::PktLobbyListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
}