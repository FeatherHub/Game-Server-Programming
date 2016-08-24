#pragma once

#include "..\..\Common\Constants.h"

struct User
{
	User() : isConnected(false) {}
	
	bool Connected() { return isConnected; }

	char name[MAX_USER_NAME_LEN];
	int clientIdx;
	bool isConnected = false;
};