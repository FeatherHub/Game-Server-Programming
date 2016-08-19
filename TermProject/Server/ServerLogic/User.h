#pragma once

#include "..\..\Common\Constants.h"

struct User
{
	User() : isConnected(false) {}
	
	bool Connected() { return isConnected; }

	wchar_t id[MAX_USER_ID_LEN];
	int clientIdx;
	bool isConnected = false;
};