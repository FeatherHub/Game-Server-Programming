#pragma once

#include "..\Common\Packetid.h"
#include "..\Common\Packet.h"
#include "..\Common\ErrorCode.h"

class PacketProcessor
{
	friend class PacketManager;

private:
	ErrorCode LoginReq(Packet& pkt, int clientId);
};