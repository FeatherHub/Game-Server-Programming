#pragma once

#include <queue>

#include "User.h"

namespace NNetworkLib
{
	class SelectNetwork;
}

class UserManager
{
	using Network = NNetworkLib::SelectNetwork;
public:
	void Init(Network* network);
	void AddUser(const char* name, int clientIdx);
	int GetUserNum() { return m_userNum; }

	//return nullptr if no matching user
	const char* FindUserName(int clientIdx);
	
	//
	User* GetUserList() { return m_userPool; }
	
	void RemoveUser(int clientIdx);
	//void RemoveUser(char);
	
	void NotifyNewbieLogin(int newbieClientIdx, char* newbieName);
	void NotifyLobbyChatMsg(int senderClientIdx, const char* senderName, const char* msg);
private:
	int GetUserPoolIdx();

private:
	std::queue<int> m_userIdxPool;
	User m_userPool[MAX_LOBBY_USER_NUM];
	int m_userNum = 0;

	Network* m_pRefNetwork;

	//map for search
};