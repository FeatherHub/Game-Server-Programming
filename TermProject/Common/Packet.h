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

//Test
struct TestReqPkt
{
	int num;
};

struct TestResPkt
{
	int num;
};

//Login
struct LoginReqPkt
{
	char name[MAX_USER_NAME_LEN];
	char pw[MAX_USER_PW_LEN];
};

struct LoginResPkt
{
	bool isPermiited;
};

struct LoginNtfPkt
{
	char newbieName[MAX_USER_NAME_LEN];
};

//LobbyUserList
struct LobbyUserIdListReqPkt
{
};

struct LobbyUserIdListResPkt //받아서 저장할 때는 link<string> 에 저장
{
	char userIdList[MAX_USER_NUM][MAX_USER_NAME_LEN];
	int userNum;
};

//LobbyChat
#pragma pack(pop)