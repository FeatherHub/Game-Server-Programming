#pragma once

#include "..\ClientLogic\ErrorCode.h"

class Client;

class ProcessManager
{
public:
	static ProcessManager* GetInstance();
private:
	static ProcessManager* m_instance;

public:
	ERRORCODE LoginRes(char* pData);
	ERRORCODE LoginNtf(char* pData, Client* pNewbie);
	ERRORCODE LobbyUserNameList(char* pData, Client** pUserArr, int* pUserNumBuf);
};