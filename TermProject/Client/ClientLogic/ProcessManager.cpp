#include "ProcessManager.h"
#include "..\..\Common\Packet.h"

#include "..\ChatProg\Classes\User.h"

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

ERRORCODE ProcessManager::LoginNtf(char* pData, User* pNewbie)
{
	auto* resPkt = (LoginNtfPkt*)pData;
	
	pNewbie->SetName(resPkt->newbieName);
	pNewbie->SetClientIdx(resPkt->newbieClientIdx);
	pNewbie->SetKey();

	return ERRORCODE::NONE;
}

ERRORCODE ProcessManager::LobbyUserNameList(char* pData, User** pUserArr, int* pUserNumBuf)
{
	auto* resPkt = (LobbyUserNameListResPkt*)pData;
	/*
	for (int i = 0; i < resPkt->userNum; i++)
	{
		pUserArr[i]->SetName();
		pUserArr[i]->SetClientIdx();
		pUserArr[i]->SetKey();
	}
	*/
	return ERRORCODE::NONE;
}