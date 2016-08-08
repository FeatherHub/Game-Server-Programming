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
		char* data;
	};
	#pragma pack(pop)

	struct RecvPacket
	{
		unsigned short id;
		char* data;
		int clientId;
	};
}