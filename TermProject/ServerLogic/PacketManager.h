#pragma once

#include "PacketProcessor.h"

#include "..\Network\NetPacket.h"
#include "..\Common\Packet.h"
#include "..\Common\Packetid.h"
#include "..\Common\ErrorCode.h"

class PacketManager
{
	using RecvPacket = NNetworkLib::RecvPacket;
	using PktProcFunc = ErrorCode(PacketProcessor::*)(Packet&, int);
	PktProcFunc m_pktFuncArray[PacketId::MAX];

public:
	void Init();

	void ProcessPacket(RecvPacket recvPkt);

private:
	PacketProcessor m_pktProcessor;
};