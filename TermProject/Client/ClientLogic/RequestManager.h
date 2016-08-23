#pragma once

#include "..\..\Common\ErrorCode.h"
#include <string>

class Network;

class RequestManager
{
public:
	static RequestManager* GetInstance();
	static RequestManager* m_instance;
private:
	RequestManager();
public:
	ERRORCODE RequestConnect(const std::string& port, const std::string& ip);
	ERRORCODE RequestLogin(const std::string& id, const std::string& pw);
private:
	Network* m_refNetwork;
};