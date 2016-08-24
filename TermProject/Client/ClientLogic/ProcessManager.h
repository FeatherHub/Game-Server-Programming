#pragma once

#include "..\ClientLogic\ErrorCode.h"

class ProcessManager
{
public:
	static ProcessManager* GetInstance();
private:
	static ProcessManager* m_instance;

public:
	ERRORCODE LoginRes(char* pData);
	ERRORCODE LoginNtf(char* pData, char* newbieNameBuff);
	ERRORCODE LobbyUserNameList(char* pData, char** userIdListBuffer);
};