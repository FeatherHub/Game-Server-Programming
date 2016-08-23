#pragma once

#include "..\..\Common\Constants.h"

struct User
{
	User() : isConnected(false) {}
	
	bool Connected() { return isConnected; }

	char id[MAX_USER_ID_LEN];
	int clientIdx;
	bool isConnected = false;
};