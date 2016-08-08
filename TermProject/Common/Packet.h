#pragma once

#pragma pack(push, 1)
struct PktHeader
{
	unsigned short id;
};

const int PACKET_HEADER_SIZE = sizeof(PktHeader);

struct TestReqPkt
{
	int num;
};

struct TestResPkt
{
	int num;
};
#pragma pack(pop)