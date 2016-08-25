#include "PktProcHeaders.h"
#include "UserManager.h"

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
		CopyMemory(ntfPkt.newbieName, newbieName, MAX_USER_NAME_LEN + 1);

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

void UserManager::NotifyRemoveUser(int removedClientIdx)
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		auto& user = m_userPool[i];
		if (user.Connected() == false)
		{
			continue;
		}

		RemoveUserNtfPkt ntfPkt;
		CopyMemory(ntfPkt.name, m_userPool[removedClientIdx].name, MAX_USER_NAME_LEN);
		m_pRefNetwork->SendPacket(user.clientIdx, PacketId::RemoveUserNtf, (char*)&ntfPkt);
	}
}
