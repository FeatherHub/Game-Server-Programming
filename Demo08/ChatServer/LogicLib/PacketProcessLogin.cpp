#include "PacketProcess.h"

#include "../ServerNetLib/TcpNetwork.h"
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"

#include "User.h"
#include "UserManager.h"
#include "LobbyManager.h"

using PACKET_ID = NCommon::PACKET_ID;

namespace NLogicLib
{
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
	CHECK_START
		//TO-DO
		//1. 로그인 패킷 크기 검사
		//2. 아이디-비밀번호 일치 검사

		auto loginPkt = (NCommon::PktLoginReq*)packetInfo.pRefData;
		
		auto ret = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, loginPkt->szID);

		if (ret != ERROR_CODE::NONE) CHECK_ERROR(ret);

		NCommon::PktLogInRes resPkt;
		resPkt.ErrorCode = (short)ret;

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
		//TO-DO: 유저가 로그인 화면에 있는지 검사

		auto pUserInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		
		auto errorCode = std::get<0>(pUserInfo);
		if (errorCode != ERROR_CODE::NONE) CHECK_ERROR(errorCode);
	
		auto pUser = std::get<1>(pUserInfo);
		if (pUser->IsCurDomainInLogIn() == false) CHECK_ERROR(ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN);
		
		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);
		
		return ERROR_CODE::NONE;

	PROCESS_ERROR:
		NCommon::PktLobbyListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
}