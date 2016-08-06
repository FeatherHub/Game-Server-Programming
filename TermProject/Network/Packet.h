#pragma once

#pragma pack(push, 1)
struct PktHeader
{
	unsigned short id = 0;
	unsigned short bodySize = 0;
};

const int PACKET_HEADER_SIZE = sizeof(PktHeader);

struct Packet
{
	unsigned short id = 0;
	unsigned short bodySize = 0;
	char* data = nullptr;
};
#pragma pack(pop)