#pragma once

#include "..\ClientLogic\ErrorCode.h"
#include <string>

class Network;

class RequestManager
{
public:
	static RequestManager* GetInstance();
private:
	RequestManager();
	static RequestManager* m_instance;
public:
	ERRORCODE RequestConnect(const std::string& port, const std::string& ip);
	ERRORCODE RequestLogin(const std::string& id, const std::string& pw);
	ERRORCODE RequestUserIdList();

private:
	Network* m_refNetwork;
};