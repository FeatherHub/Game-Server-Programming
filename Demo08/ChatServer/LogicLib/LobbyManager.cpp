#include "../ServerNetLib/ILog.h"
#include "../ServerNetLib/TcpNetwork.h"
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"

#include "Lobby.h"
#include "LobbyManager.h"

using ERROR_CODE = NCommon::ERROR_CODE;
using PACKET_ID = NCommon::PACKET_ID;

namespace NLogicLib
{
	LobbyManager::LobbyManager() {}

	LobbyManager::~LobbyManager() {}


	void LobbyManager::Init(const LobbyManagerConfig config, TcpNet* pNetwork, ILog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;

		for (int i = 0; i < config.MaxLobbyCount; ++i)
		{
			Lobby lobby;
			lobby.Init((short)i, (short)config.MaxLobbyUserCount, (short)config.MaxRoomCountByLobby, (short)config.MaxRoomUserCount);
			lobby.SetNetwork(m_pRefNetwork, m_pRefLogger);

			m_LobbyList.push_back(lobby);
		}
	}

	Lobby* LobbyManager::GetLobby(short lobbyId)
	{
		if (lobbyId < 0 || lobbyId >= (short)m_LobbyList.size()) {
			return nullptr;
		}

		return &m_LobbyList[lobbyId];
	}
		
	void LobbyManager::SendLobbyListInfo(const int sessionIndex)
	{
		NCommon::PktLobbyListRes pktToSend;
		pktToSend.ErrorCode = (short)ERROR_CODE::NONE;
		pktToSend.LobbyCount = (short)m_LobbyList.size();

		for (int lobbyIdx = 0, length = m_LobbyList.size(); lobbyIdx < length; lobbyIdx++)
		{
			auto lobby = m_LobbyList[lobbyIdx];

			pktToSend.LobbyListInfoArr[lobbyIdx].LobbyId = lobby.GetIndex();
			pktToSend.LobbyListInfoArr[lobbyIdx].LobbyUserCount = lobby.GetUserCount();
		}

		m_pRefNetwork->SendData(sessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(pktToSend), (char*)&pktToSend);
	}
}