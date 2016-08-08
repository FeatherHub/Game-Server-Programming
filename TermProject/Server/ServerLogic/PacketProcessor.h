#pragma once

namespace NNetworkLib
{
	class SelectNetwork;
	struct Packet;
}

enum ERRORCODE : int;

class PacketProcessor
{
	friend class PacketManager;
	using Network = NNetworkLib::SelectNetwork;
	using Packet = NNetworkLib::Packet;

public:
	void Init(Network* network);

private:
	ERRORCODE TestReq(char* data, int clientId);
	ERRORCODE LoginReq(char* data, int clientId);

private:
	Network* m_pRefNetwork;
};