#pragma once

#include "..\..\Common\Packetid.h"
#include "..\Network\NetPacket.h"

class IOCPNetwork;

enum ERRORCODE : int;

class PacketProcessor;
class UserManager;

class PacketProcArray
{
	using Network = IOCPNetwork;
	using RecvPacket = NetworkLib::NetPacket;

	using PktProcFunc = ERRORCODE(PacketProcessor::*)(char*, int);
	PktProcFunc m_pktFuncArray[(int)PacketId::MAX];

public:
	void Init(Network* network, UserManager* userMgr);
	void ProcessPacket(RecvPacket& recvPkt);
private:
	void RegisterPktProcArr();
private:
	PacketProcessor* m_pPktProcessor;
	Network* m_pRefNetwork;
};