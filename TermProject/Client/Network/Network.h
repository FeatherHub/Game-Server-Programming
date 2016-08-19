#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

class Network
{
public:
	bool Init();
	bool Connect();
private:
	SOCKET m_socket;

};