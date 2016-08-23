#pragma once

#include "Constants.h"

#pragma pack(push, 1)
struct PktHeader
{
	unsigned short id;
};

const int PACKET_HEADER_SIZE = sizeof(PktHeader);

struct Packet
{
	unsigned short id;
	char* pDataAtBuff;
};

struct TestReqPkt
{
	int num;
};

struct TestResPkt
{
	int num;
};


struct LoginReqPkt
{
	char id[MAX_USER_ID_LEN];
	char pw[MAX_USER_PW_LEN];
};

struct LoginResPkt
{
	bool isPermiited;
};

#pragma pack(pop)