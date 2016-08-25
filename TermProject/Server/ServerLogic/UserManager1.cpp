#include "PktProcHeaders.h"
#include "UserManager.h"

void UserManager::Init(Network* network)
{
	m_pRefNetwork = network;

	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		m_userIdxPool.push(i);
	}
}

void UserManager::AddUser(const char* name, int clientIdx)
{
	int userIdx = GetUserPoolIdx();
	
	User& user = m_userPool[userIdx];
	
	user.isConnected = true;
	user.clientIdx = clientIdx;

	CopyMemory(user.name, name, strlen(name));
	user.name[strlen(name)] = '\0';

	m_userNum++;

	Logger::Write(Logger::INFO, "User Num : %d Cur Logined User %s ",m_userNum, name);
}

const char* UserManager::FindUserName(int clientIdx)
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		const auto& user = m_userPool[i];
		if (user.Connected() == true)
		{
			if (user.GetClientIdx() == clientIdx)
			{
				return user.GetName();
			}
		}
	}

	return nullptr;
}

void UserManager::RemoveUser(int clientIdx)
{
	auto& user = m_userPool[clientIdx];
	user.isConnected = false;
	user.clientIdx = -1;
	
	m_userNum--;
	
	m_userIdxPool.push(clientIdx);

	m_pRefNetwork->BanClient(clientIdx);
}

int UserManager::GetUserPoolIdx()
{
	int idx = m_userIdxPool.front();
	m_userIdxPool.pop();

	return idx;
}
