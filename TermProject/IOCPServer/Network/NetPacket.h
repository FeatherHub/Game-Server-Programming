#pragma once

namespace NetworkLib
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

	struct NetPacket
	{
		unsigned short id = 0;
		char* pDataAtBuff = nullptr;
		int clientIdx = -1;
	};
}