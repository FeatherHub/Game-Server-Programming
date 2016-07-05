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
	PacketProcess::PacketProcess() {}
	PacketProcess::~PacketProcess() {}

	void PacketProcess::Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ILog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
		m_pRefUserMgr = pUserMgr;
		m_pRefLobbyMgr = pLobbyMgr;

		using netLibPacketId = NServerNetLib::PACKET_ID;
		using commonPacketId = NCommon::PACKET_ID;
		for (int i = 0; i < (int)commonPacketId::MAX; ++i)
		{
			PacketFuncArray[i] = nullptr;
		}

		PacketFuncArray[(int)netLibPacketId::NTF_SYS_CLOSE_SESSION] = &PacketProcess::NtfSysCloseSesson;
		PacketFuncArray[(int)commonPacketId::LOGIN_IN_REQ] = &PacketProcess::Login;
		PacketFuncArray[(int)commonPacketId::LOBBY_LIST_REQ] = &PacketProcess::LobbyList;
		PacketFuncArray[(int)commonPacketId::LOBBY_ENTER_REQ] = &PacketProcess::LobbyEnter;
		PacketFuncArray[(int)commonPacketId::LOBBY_ENTER_ROOM_LIST_REQ] = &PacketProcess::LobbyRoomList;
		PacketFuncArray[(int)commonPacketId::LOBBY_ENTER_USER_LIST_REQ] = &PacketProcess::LobbyUserList;
		PacketFuncArray[(int)commonPacketId::LOBBY_LEAVE_REQ] = &PacketProcess::LobbyLeave;
		PacketFuncArray[(int)commonPacketId::ROOM_ENTER_REQ] = &PacketProcess::RoomEnter;
		PacketFuncArray[(int)commonPacketId::ROOM_LEAVE_REQ] = &PacketProcess::RoomLeave;
		PacketFuncArray[(int)commonPacketId::ROOM_CHAT_REQ] = &PacketProcess::RoomChat;
	}
	
	void PacketProcess::Process(PacketInfo packetInfo)
	{
		auto packetId = packetInfo.PacketId;
		
		if (PacketFuncArray[packetId] == nullptr)
		{
			//TODO: 로그 남긴다
		}

		(this->*PacketFuncArray[packetId])(packetInfo);
	}

	ERROR_CODE PacketProcess::NtfSysCloseSesson(PacketInfo packetInfo)
	{
		auto pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));

		if (pUser) 
		{
			auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
			if (pLobby)
			{
				auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());

				if (pRoom)
				{
					pRoom->LeaveUser(pUser->GetIndex());
					pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());
					pLobby->NotifyChangedRoomInfo(pRoom->GetIndex());

					m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d). Room Out", __FUNCTION__, packetInfo.SessionIndex);
				}

				pLobby->LeaveUser(pUser->GetIndex());

				if (pRoom == nullptr) {
					pLobby->NotifyLobbyLeaveUserInfo(pUser);
				}

				m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d). Lobby Out", __FUNCTION__, packetInfo.SessionIndex);
			}
			
			m_pRefUserMgr->RemoveUser(packetInfo.SessionIndex);		}
		

		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);
		return ERROR_CODE::NONE;
	}
	
}