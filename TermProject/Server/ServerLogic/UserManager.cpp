#include "PktProcHeaders.h"
#include "UserManager.h"

void UserManager::Init(Network* network)
{
	m_pRefNetwork = network;

	for (int i = 0; i < MAX_USER_NUM; i++)
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
	strncpy(user.name, id, strlen(id));

	m_userNum++;
}

int UserManager::GetUserPoolIdx()
{
	int idx = m_userIdxPool.back();
	m_userIdxPool.pop();

	return idx;
}

void UserManager::RemoveUser(int clientIdx)
{
	auto& user = m_userPool[clientIdx];
	user.isConnected = false;
	user.clientIdx = 0;

	m_userNum--;

	m_userIdxPool.push(clientIdx);
}

void UserManager::NotifyNewbieLogin(int newbieClientIdx, char* newbieName)
{
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (m_userPool[i].Connected() == false)
		{
			continue;
		}

		int otherClientIdx = m_userPool[i].clientIdx;

		if (otherClientIdx == newbieClientIdx)
		{
			continue;
		}

		m_pRefNetwork->SendPacket(otherClientIdx, PacketId::LoginNtf, newbieName);
	}
}