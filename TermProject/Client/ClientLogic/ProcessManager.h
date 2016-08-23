#pragma once

#include "..\ClientLogic\ErrorCode.h"

class ProcessManager
{
public:
	static ProcessManager* GetInstance();
private:
	static ProcessManager* m_instance;
	ProcessManager();

public:
	ERRORCODE ProcessLogin(char* dataPos);

};