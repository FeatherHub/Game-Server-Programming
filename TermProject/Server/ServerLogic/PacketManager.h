#pragma once

#include "PacketProcessor.h"

#include "..\..\Common\Packetid.h"
#include "..\Network\NetPacket.h"

namespace NNetworkLib
{
	class SelectNetwork;
}

enum ERRORCODE : int;

class PacketManager
{
	using Network = NNetworkLib::SelectNetwork;
	using RecvPacket = NNetworkLib::RecvPacket;

	using PktProcFunc = ERRORCODE(PacketProcessor::*)(char*, int);
	PktProcFunc m_pktFuncArray[(int)PacketId::MAX];

public:
	void Init(Network* network);

	void ProcessPacket(RecvPacket& recvPkt);

private:
	PacketProcessor m_pktProcessor;
	Network* m_pRefNetwork;
};