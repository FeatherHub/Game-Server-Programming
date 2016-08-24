#pragma once

namespace NNetworkLib
{
	class SelectNetwork;
}

class UserManager;

enum ERRORCODE : int;

class PacketProcessor
{
	friend class PacketManager;
	using Network = NNetworkLib::SelectNetwork;
public:
	void Init(Network* network);

private:
	ERRORCODE TestReq(char* pData, int clientIdx);
	ERRORCODE LoginReq(char* pData, int clientIdx);
	ERRORCODE LobbyUserListReq(char* pData, int clientIdx);

private:
	Network* m_pRefNetwork;
	UserManager* m_pUserManager;
};