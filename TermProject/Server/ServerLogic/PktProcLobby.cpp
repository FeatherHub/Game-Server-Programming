#include "PktProcHeaders.h"

ERRORCODE PacketProcessor::LobbyUserListReq(char* pData, int clientIdx)
{
	auto reqPkt = (LobbyUserIdListReqPkt*)pData;
	
	//유효성 검사
	
	//처리
	LobbyUserIdListResPkt resPkt;

	auto* userList = m_pUserManager->GetUserList();
	int cnt = 0;
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (userList[i].Connected() == true)
		{
			CopyMemory(resPkt.userIdList[cnt++], userList[i].name, MAX_USER_NAME_LEN);
		}
	}
	resPkt.userNum = cnt;

	m_pRefNetwork->SendPacket(clientIdx, PacketId::LobbyUserNameListRes, (char*)&resPkt);

	return ERRORCODE::NONE;
}
