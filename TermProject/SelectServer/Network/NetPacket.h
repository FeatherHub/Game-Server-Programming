#pragma once

namespace NNetworkLib
{
	#pragma pack(push, 1)
	struct PktHeader
	{
		unsigned short id;
	};

	const int PACKET_HEADER_SIZE = sizeof(PktHeader);

	struct Packet
	{
		unsigned short id;
		char* pData;
	};
	#pragma pack(pop)

	struct RecvPacket
	{
		unsigned short id;
		char* pDataAtBuff;
		int clientId;
	};
}