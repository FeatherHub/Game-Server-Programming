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
	char name[MAX_USER_NAME_LEN + 1];
	char pw[MAX_USER_PW_LEN + 1];
};

struct LoginResPkt
{
	bool isPermiited;
};

struct LoginNtfPkt
{
	char newbieName[MAX_USER_NAME_LEN + 1];
};

//LobbyUserList
struct LobbyUserNameListReqPkt
{
};

struct LobbyUserNameListResPkt //받아서 저장할 때는 link<string> 에 저장
{
	char userNameList[MAX_LOBBY_USER_NUM][MAX_USER_NAME_LEN + 1]; //+1 for '\0'
	int userNum;
};

//LobbyChat
struct LobbyChatReqPkt
{
	char msg[MAX_CHAT_MSG_LEN + 1];
};

struct LobbyChatResPkt
{
	bool permitted;
};

struct LobbyChatNtfPkt 
{
	char senderName[MAX_USER_NAME_LEN + 1];
	char msg[MAX_CHAT_MSG_LEN + 1];
};
#pragma pack(pop)