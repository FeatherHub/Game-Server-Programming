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
	m_pRefNetwork = Network::GetInstance();
}

ERRORCODE RequestManager::RequestConnect(const std::string& port, const std::string& ip)
{
	unsigned short portNumber = atoi(port.c_str());

	auto res = m_pRefNetwork->ConnectTo(ip.c_str(), portNumber);

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

	if (name.length() > MAX_USER_NAME_LEN || pw.length() > MAX_USER_PW_LEN)
	{
		return ERRORCODE::LOGIN_REQ_ID_OR_PW_OVERFLOW;
	}

	/* 이후에 최적화하기
	/* SendPacket에 가변인자로 데이터의 위치를 받아서
	/* 복사작업은 SendPacket에서 일괄적으로 처리한다
	*/

	LoginReqPkt reqPkt;

	CopyMemory(reqPkt.name, name.c_str(), name.length());
	reqPkt.name[name.length()] = '\0';

	CopyMemory(reqPkt.pw, pw.c_str(), pw.length());
	reqPkt.pw[pw.length()] = '\0';

	m_pRefNetwork->SendPacket(PacketId::LoginReq, (char*)&reqPkt);
	
	return ERRORCODE::LOGIN_REQ_OK;
}

ERRORCODE RequestManager::RequestUserNameList()
{
	LobbyUserNameListReqPkt reqPkt;
	
	m_pRefNetwork->SendPacket(PacketId::LobbyUserNameListReq, (char*)&reqPkt);
	
	return ERRORCODE::LOBBY_USER_ID_LIST_REQ_OK;
}

ERRORCODE RequestManager::RequestLobbyChat(const std::string& szMsg)
{
	LobbyChatReqPkt reqPkt;
	
	if (szMsg.length() == 0)
	{
		return ERRORCODE::LOBBY_CHAT_REQ_NO_MSG;
	}

	if (szMsg.length() > MAX_CHAT_MSG_LEN)
	{
		return ERRORCODE::LOBBY_CHAT_REQ_OVERFLOW_MSG;
	}

	CopyMemory(reqPkt.msg, szMsg.c_str(), szMsg.length());
	reqPkt.msg[szMsg.length()] = '\0';

	m_pRefNetwork->SendPacket(PacketId::LobbyChatReq, (char*)&reqPkt);

	return ERRORCODE::LOBBY_CHAT_REQ_OK;
}