#pragma once

#include "..\Network\NetPacket.h"
#include "..\Common\Packetid.h"
#include "..\Common\Packet.h"
#include "..\Common\ErrorCode.h"

class PacketProcessor;

class PacketManager
{
	using RecvPacket = NNetworkLib::RecvPacket;
	using PktProcFunc = ErrorCode(PacketProcessor::*)(Packet&, int);

public:
	void Init();

	void ProcessPacket(RecvPacket recvPkt);

private:
	PktProcFunc m_pktFuncArray[PacketId::MAX];
};