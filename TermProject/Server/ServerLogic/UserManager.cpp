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
}

int UserManager::GetUserPoolIdx()
{
	int idx = m_userIdxPool.back();
	m_userIdxPool.pop();

	return idx;
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
	user.clientIdx = 0;

	m_userNum--;

	m_userIdxPool.push(clientIdx);
}

void UserManager::NotifyNewbieLogin(int newbieClientIdx, char* newbieName)
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
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

		LoginNtfPkt ntfPkt;
		CopyMemory(ntfPkt.newbieName, newbieName, MAX_USER_NAME_LEN);

		m_pRefNetwork->SendPacket(otherClientIdx, (unsigned short)PacketId::LoginNtf, (char*)&ntfPkt);
	}
}

void UserManager::NotifyLobbyChatMsg(int senderClientIdx, const char* senderName, const char* msg)
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		const auto& user = m_userPool[i];
		if (user.Connected() == false)
		{
			continue;
		}

		if (user.GetClientIdx() == senderClientIdx)
		{
			continue;
		}

		LobbyChatNtfPkt ntfPkt;
		CopyMemory(ntfPkt.senderName, senderName, MAX_USER_NAME_LEN);
		CopyMemory(ntfPkt.msg, msg, MAX_CHAT_MSG_LEN);

		m_pRefNetwork->SendPacket(user.GetClientIdx(), PacketId::LobbyChatNtf, (char*)&ntfPkt);
	}
}
