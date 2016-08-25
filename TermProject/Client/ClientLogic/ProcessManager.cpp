#include "ProcessManager.h"
#include "..\..\Common\Packet.h"

#include <Windows.h> //CopyMemory

ProcessManager* ProcessManager::m_instance = nullptr;

ProcessManager* ProcessManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new ProcessManager();
	}

	return m_instance;
}

ERRORCODE ProcessManager::LoginRes(char* pData)
{
	auto* pkt = (LoginResPkt*)pData;

	return pkt->isPermiited ? ERRORCODE::LOGIN_RES_OK : ERRORCODE::LOGIN_RES_NO;
}

ERRORCODE ProcessManager::LoginNtf(char* pData, char* newbieNameBuff)
{
	auto* resPkt = (LoginNtfPkt*)pData;
	
	CopyMemory(newbieNameBuff, resPkt->newbieName, MAX_USER_NAME_LEN);

	return ERRORCODE::NONE;
}

ERRORCODE ProcessManager::LobbyUserNameList(char* pData, char* userNameListBuff, int* pUserNumBuff)
{
	auto* resPkt = (LobbyUserNameListResPkt*)pData;

	CopyMemory(userNameListBuff, (char*)resPkt->userNameList, (resPkt->userNum * (MAX_USER_NAME_LEN+1)));
	
	*pUserNumBuff = resPkt->userNum;

	return ERRORCODE::NONE;
}