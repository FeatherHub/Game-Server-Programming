#include "ProcessManager.h"
#include "..\..\Common\Packet.h"

#include "..\ChatProg\Classes\Client.h"

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

ERRORCODE ProcessManager::LoginNtf(char* pData, Client* pNewbie)
{
	auto* resPkt = (LoginNtfPkt*)pData;
	
	pNewbie->SetName(resPkt->newbieName);
	pNewbie->SetClientIdx(resPkt->newbieClientIdx);
	pNewbie->SetNewKey();
	pNewbie->Connect();

	return ERRORCODE::NONE;
}

ERRORCODE ProcessManager::LobbyUserNameList(char* pData, Client** pUserArr, int* pUserNumBuf)
{
	auto* resPkt = (LobbyUserNameListResPkt*)pData;

	for (int i = 0; i < resPkt->userNum; i++)
	{
		int clientIdx = resPkt->userClientIdxList[i];
		pUserArr[clientIdx]->SetName(resPkt->userNameList[i]);
		pUserArr[clientIdx]->SetClientIdx(clientIdx);
		pUserArr[clientIdx]->SetNewKey();
		pUserArr[clientIdx]->Connect();
	}
	
	*pUserNumBuf = resPkt->userNum;

	return ERRORCODE::NONE;
}