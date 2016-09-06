#pragma once

#include <queue>

#include "User.h"

class IOCPNetwork;

class UserManager
{
	using Network = IOCPNetwork;
public:
	void Init(Network* network);
	void AddUser(const char* name, int clientIdx);
	int GetUserNum() { return m_userNum; }

	//return nullptr if no matching user
	const char* FindUserName(int clientIdx);
	
	//
	User* GetUserList() { return m_userPool; }

	void RemoveUser(int clientIdx);
	//void RemoveUser(char* name);

	void NotifyRemoveUser(int removedClientIdx);
	void NotifyNewbieLogin(int newbieClientIdx, char* newbieName);
	void NotifyLobbyChatMsg(int senderClientIdx, const char* msg);
private:
	Network* m_pRefNetwork;
	
	User m_userPool[MAX_LOBBY_USER_NUM];
	int m_userNum = 0;
};