#pragma once

#include <queue>

#include "User.h"

class UserManager
{
public:
	void Init();
	void AddUser(wchar_t* id, int clientIdx);
	//void RemoveUser(int)
	//void RemoveUser(wchar_t)
private:
	enum { MAX_USER_NUMBER = 1024 };

	int GetUserPoolIdx();
	std::queue<int> m_userIdxPool;
	User m_userPool[ MAX_USER_NUMBER ];

	//map for search
};