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
	void AddUser(char* id, int clientIdx);
	int GetUserNum() { return m_userNum; }
	void RemoveUser(int clientIdx);
	//void RemoveUser(wchar_t);
	
	void NotifyNewbieLogin(int newbieClientIdx, char* newbieName);
	User* GetUserList() { return m_userPool; }
private:
	int GetUserPoolIdx();

private:
	std::queue<int> m_userIdxPool;
	User m_userPool[MAX_USER_NUM];
	int m_userNum = 0;

	Network* m_pRefNetwork;

	//map for search
};