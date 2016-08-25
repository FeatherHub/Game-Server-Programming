#pragma once

enum PacketId : unsigned short
{
	TestReq = 30,
	TestRes = 31,
	
	LoginReq = 35,
	LoginRes = 36,
	LoginNtf = 37,

	LobbyChatReq = 40,
	LobbyChatRes = 41,
	LobbyChatNtf = 42,

	LobbyUserNameListReq = 50,
	LobbyUserNameListRes = 51,

	RemoveUserNtf = 60,

	MAX = 512
};