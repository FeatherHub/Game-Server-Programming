#include "RequestManager.h"
#include "..\Network\Network.h"

#include "..\..\Common\Packet.h"
#include "..\..\Common\PacketId.h"

RequestManager* RequestManager::m_instance = nullptr;

RequestManager* RequestManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new RequestManager();
	}

	return m_instance;
}

RequestManager::RequestManager()
{
	m_refNetwork = Network::GetInstance();
}

ERRORCODE RequestManager::RequestConnect(const std::string& port, const std::string& ip)
{
	unsigned short portNumber = atoi(port.c_str());

	auto res = m_refNetwork->ConnectTo(ip.c_str(), portNumber);

	if (res == true)
	{
		return ERRORCODE::CONNECT_REQ_OK;
	}
	else
	{
		return ERRORCODE::CONNECT_REQ_FAIL;
	}
}

ERRORCODE RequestManager::RequestLogin(const std::string& name, const std::string& pw)
{
	if (name.length() == 0 || pw.length() == 0)
	{
		return ERRORCODE::LOGIN_REQ_ID_OR_PW_EMPTY;
	}

	LoginReqPkt reqPkt;

	/* 이후에 최적화하기
	/* SendPacket에 가변인자로 데이터의 위치를 받아서
	/* 복사작업은 SendPacket에서 일괄적으로 처리한다
	*/

	CopyMemory(&reqPkt.name, &name, name.length());
	reqPkt.name[name.length()] = '\0';

	CopyMemory(&reqPkt.pw, &pw, pw.length());
	reqPkt.name[pw.length()] = '\0';

	m_refNetwork->SendPacket(PacketId::LoginReq, (char*)&reqPkt);
	
	return ERRORCODE::LOGIN_REQ_OK;
}

ERRORCODE RequestManager::RequestUserIdList()
{
	LobbyUserIdListReqPkt reqPkt;
	
	m_refNetwork->SendPacket(PacketId::LobbyUserIdListReq, (char*)&reqPkt);
	
	return ERRORCODE::LOBBY_USER_ID_LIST_REQ_OK;
}
