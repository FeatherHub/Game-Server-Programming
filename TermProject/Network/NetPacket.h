#pragma once

namespace NNetworkLib
{
	#pragma pack(push, 1)
	struct PktHeader
	{
		unsigned short id;
		unsigned short bodySize;
	};

	const int PACKET_HEADER_SIZE = sizeof(PktHeader);

	struct Packet
	{
		unsigned short id;
		unsigned short bodySize;
		char* data;
	};
	#pragma pack(pop)

	struct RecvPacket
	{
		unsigned short id;
		unsigned short bodySize;
		char* data;
		int clientId;
	};
}