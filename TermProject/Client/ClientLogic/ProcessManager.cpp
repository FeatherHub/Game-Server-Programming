#include "ProcessManager.h"
#include "..\..\Common\Packet.h"

ProcessManager* ProcessManager::m_instance = nullptr;

ProcessManager* ProcessManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new ProcessManager();
	}

	return m_instance;
}

ProcessManager::ProcessManager()
{

}

ERRORCODE ProcessManager::ProcessLogin(char* dataPos)
{
	auto* pkt = (LoginResPkt*)dataPos;

	if (pkt->isPermiited == true)
	{
		return ERRORCODE::LOGIN_RES_OK;
	}
	else
	{
		return ERRORCODE::LOGIN_RES_NO;
	}
}
