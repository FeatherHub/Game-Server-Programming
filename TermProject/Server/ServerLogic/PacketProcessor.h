#pragma once

namespace NNetworkLib
{
	class SelectNetwork;
	struct Packet;
}

class UserManager;

enum ERRORCODE : int;

class PacketProcessor
{
	friend class PacketManager;
	using Network = NNetworkLib::SelectNetwork;
	using Packet = NNetworkLib::Packet;

public:
	void Init(Network* network);

private:
	ERRORCODE TestReq(char* pData, int clientId);
	ERRORCODE LoginReq(char* pData, int clientId);

private:
	Network* m_pRefNetwork;
	UserManager* m_pUserManager;
};