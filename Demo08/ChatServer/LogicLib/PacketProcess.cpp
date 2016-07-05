#include "../ServerNetLib/ILog.h"
#include "../ServerNetLib/TcpNetwork.h"

#include "User.h"
#include "UserManager.h"
#include "Room.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using LOG_TYPE = NServerNetLib::LOG_TYPE;

namespace NLogicLib
{
	void PacketProcess::Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ILog* pLogger)
	{
		m_pRefNetwork = pNetwork;

		m_pRefUserMgr = pUserMgr;

		m_pRefLobbyMgr = pLobbyMgr;

		m_pRefLogger = pLogger;

		using NetLibPacketId = NServerNetLib::PACKET_ID;
		using CommonPacketId = NCommon::PACKET_ID;

		for (int i = 0; i < CommonPacketId::MAX; ++i)
		{
			PacketFuncArray[i] = nullptr;
		}

		PacketFuncArray[NetLibPacketId::NTF_SYS_CLOSE_SESSION] = PacketProcess::NtfSysCloseSesson;
		PacketFuncArray[CommonPacketId::LOGIN_IN_REQ] = PacketProcess::Login;
		PacketFuncArray[CommonPacketId::LOBBY_LIST_REQ] = PacketProcess::LobbyList;
		PacketFuncArray[CommonPacketId::LOBBY_ENTER_REQ] = PacketProcess::LobbyEnter;
		PacketFuncArray[CommonPacketId::LOBBY_ENTER_ROOM_LIST_REQ] = PacketProcess::LobbyRoomList;
		PacketFuncArray[CommonPacketId::LOBBY_ENTER_USER_LIST_REQ] = PacketProcess::LobbyUserList;
		PacketFuncArray[CommonPacketId::LOBBY_LEAVE_REQ] = PacketProcess::LobbyLeave;
		PacketFuncArray[CommonPacketId::ROOM_ENTER_REQ] = PacketProcess::RoomEnter;
		PacketFuncArray[CommonPacketId::ROOM_LEAVE_REQ] = PacketProcess::RoomLeave;
		PacketFuncArray[CommonPacketId::ROOM_CHAT_REQ] = PacketProcess::RoomChat;
	}

	void PacketProcess::Process(PacketInfo packetInfo)
	{
		auto packetId = packetInfo.PacketId;

		if (PacketFuncArray[packetId] == nullptr) { return; }

		(this->*PacketFuncArray[packetId])(packetInfo);
	}

	ERROR_CODE PacketProcess::NtfSysCloseSesson(PacketInfo packetInfo)
	{
		auto pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));

		if (pUser == nullptr) { return ERROR_CODE::NONE; }

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());

		if (pLobby == nullptr) { return ERROR_CODE::NONE; }

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());

		if (pRoom == nullptr) { return ERROR_CODE::NONE; }

		//Room Out
		pRoom->LeaveUser(pUser->GetIndex());
		pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());		
		pLobby->NotifyChangedRoomInfo(pRoom->GetIndex());
		
		//Lobby Out
		pLobby->LeaveUser(pUser->GetIndex());
		pLobby->NotifyLobbyLeaveUserInfo(pUser);

		//User Out
		m_pRefUserMgr->RemoveUser(packetInfo.SessionIndex);

		m_pRefLogger->Write(LOG_TYPE::L_INFO, 
			"%s | NtfSysCloseSesson. sessionIndex(%d)", 
			__FUNCTION__, packetInfo.SessionIndex);
		
		return ERROR_CODE::NONE;
	}
}