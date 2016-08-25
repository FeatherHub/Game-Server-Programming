#include "PktProcHeaders.h"

ERRORCODE PacketProcessor::LobbyUserListReq(char* pData, int clientIdx)
{
//	auto reqPkt = (LobbyUserNameListReqPkt*)pData; //dummy packet
	
	//처리
	LobbyUserNameListResPkt resPkt;

	auto* userList = m_pUserManager->GetUserList();
	int cnt = 0;
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		if (userList[i].Connected() == true)
		{
			CopyMemory(&resPkt.userNameList[cnt][0], userList[i].name, MAX_USER_NAME_LEN+1);
			cnt++;
		}
	}
	resPkt.userNum = cnt;

	m_pRefNetwork->SendPacket(clientIdx, (unsigned short)PacketId::LobbyUserNameListRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}

ERRORCODE PacketProcessor::LobbyChatReq(char* pData, int clientIdx)
{
	auto* reqPkt = (LobbyChatReqPkt*)pData;
	
	//데이터 가져오기 및 유효성 검사
	const auto* senderName = m_pUserManager->FindUserName(clientIdx);
	if (senderName == nullptr)
	{
		return ERRORCODE::LOBBY_CHAT_SENDER_NAME_NULL;
	}

	//처리
	LobbyChatResPkt resPkt;
	resPkt.permitted = true;
	
	m_pRefNetwork->SendPacket(clientIdx, PacketId::LobbyChatRes, (char*)&resPkt);

	//통지
	m_pUserManager->NotifyLobbyChatMsg(clientIdx, senderName, reqPkt->msg);

	return ERRORCODE::NONE;
}
