#pragma once

#include "..\ClientLogic\ErrorCode.h"

class User;

class ProcessManager
{
public:
	static ProcessManager* GetInstance();
private:
	static ProcessManager* m_instance;

public:
	ERRORCODE LoginRes(char* pData);
	ERRORCODE LoginNtf(char* pData, User* pNewbie);
	ERRORCODE LobbyUserNameList(char* pData, User** pUserArr, int* pUserNumBuf);
};