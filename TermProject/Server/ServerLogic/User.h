#pragma once

#include "..\..\Common\Constants.h"

struct User
{
	User() : isConnected(false) {}
	
	bool Connected() const { return isConnected; }
	int GetClientIdx() const { return clientIdx; }
	const char* GetName() const { return name; }
	char name[MAX_USER_NAME_LEN + 1];
	int clientIdx;
	bool isConnected = false;
};