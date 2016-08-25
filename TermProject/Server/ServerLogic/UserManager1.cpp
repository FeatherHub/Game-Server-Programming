#include "PktProcHeaders.h"
#include "UserManager.h"

void UserManager::Init(Network* network)
{
	m_pRefNetwork = network;
}

void UserManager::AddUser(const char* name, int clientIdx)
{
	User& user = m_userPool[clientIdx];
	
	user.isConnected = true;
	user.clientIdx = clientIdx;

	CopyMemory(user.name, name, strlen(name));
	user.name[strlen(name)] = '\0';

	m_userNum++;

	Logger::Write(Logger::INFO, "User Num : %d Name : %s ",m_userNum, name);
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
	
	m_pRefNetwork->ForceCloseClient(clientIdx);
	NotifyRemoveUser(clientIdx);

	m_userNum--;
}