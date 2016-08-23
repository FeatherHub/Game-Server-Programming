#include "UserManager.h"

void UserManager::Init()
{
	for (int i = 0; i < MAX_USER_NUMBER; i++)
	{
		m_userIdxPool.push(i);
	}
}

void UserManager::AddUser(char* id, int clientIdx)
{
	int userIdx = GetUserPoolIdx();
	
	User& user = m_userPool[userIdx];
	
	user.isConnected = true;
	user.clientIdx = clientIdx;
//	wcsncpy(user.id, id, wcslen(id));
	strncpy(user.id, id, strlen(id));
}

int UserManager::GetUserPoolIdx()
{
	int idx = m_userIdxPool.back();
	m_userIdxPool.pop();

	return idx;
}
