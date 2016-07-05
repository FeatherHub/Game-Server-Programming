#pragma once

#include "../../Common/Packet.h"
#include "../ServerNetLib/Define.h"
#include "../../Common/ErrorCode.h"

using ERROR_CODE = NCommon::ERROR_CODE;

namespace NServerNetLib
{
	class ITcpNetwork;
}

namespace NServerNetLib
{
	class ILog;
}


namespace NLogicLib
{	
	class UserManager;
	class LobbyManager;

	#define CHECK_START  ERROR_CODE __result=ERROR_CODE::NONE;
	#define CHECK_ERROR(f) __result=f; goto CHECK_ERR;

	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		typedef ERROR_CODE(PacketProcess::*PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

		using TcpNet = NServerNetLib::ITcpNetwork;
		using ILog = NServerNetLib::ILog;

	public:
		PacketProcess();
		~PacketProcess();

		void Init(TcpNet* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ILog* pLogger);

		void Process(PacketInfo packetInfo);
	
	private:
		ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;
		UserManager* m_pRefUserMgr;
		LobbyManager* m_pRefLobbyMgr;
		
				
	private:
		ERROR_CODE NtfSysCloseSesson(PacketInfo packetInfo);
		
		ERROR_CODE Login(PacketInfo packetInfo);
		
		ERROR_CODE LobbyList(PacketInfo packetInfo);

		ERROR_CODE LobbyEnter(PacketInfo packetInfo);

		ERROR_CODE LobbyRoomList(PacketInfo packetInfo);

		ERROR_CODE LobbyUserList(PacketInfo packetInfo);

		ERROR_CODE LobbyLeave(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);

		ERROR_CODE RoomChat(PacketInfo packetInfo);
	};
}