#include "LobbyScene.h"
#include "Constants.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

////////////////////////////////////////////////////////////////
// ProcessManager와 연동하여 Scene의 데이터와 UI를 갱신한다.
////////////////////////////////////////////////////////////////

void LobbyScene::LoginNtf(char* pData)
{
	ProcessManager::GetInstance()->LoginNtf(pData, m_userNameList[m_userNum]);

	AddUserNameToList(m_userNameList[m_userNum]);
	CCLOG("Newbie Name : %s", m_userNameList[m_userNum]);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	int userListNum = 0;
	ProcessManager::GetInstance()->LobbyUserNameList(pData, (char*)m_userNameList, &userListNum);

	for (int i = 0; i < userListNum; i++)
	{
		AddUserNameToList(m_userNameList[i]);
		CCLOG("Name %d : %s", i, m_userNameList[i]);
	}
}

void LobbyScene::LobbyChatRes(char* pData)
{

}

void LobbyScene::LobbyChatNtf(char* pData)
{

}

void LobbyScene::RemoveUserNtf(char* pData)
{
	RemoveUserNtfPkt* ntfPkt = (RemoveUserNtfPkt*)pData;

	for (int i = 0; i < m_userNum; i++)
	{
		if (strcmp(m_userNameList[i], ntfPkt->name) == 0)
		{
			if (i != m_userNum - 1) // case 1
			{
				m_labelNameArr[m_userNum-1]->setPosition(Point(0, -((i+1) * Constants::USER_NAME_POS_DELTA)));

				return;
			}

			//remove ui
			auto* childUi = m_nodeUserName->getChildByName(ntfPkt->name);
			childUi->removeFromParent();
			
			m_userNum--;
		}
	}
}

void LobbyScene::AddUserNameToList(const char* userName)
{
	m_labelNameArr[m_userNum] = Label::create(userName, Constants::DEFAULT_FONT, Constants::USER_NAME_FONT_SIZE);
	m_labelNameArr[m_userNum]->setPosition(Point(0, -((m_userNum+1) * Constants::USER_NAME_POS_DELTA)));
	m_labelNameArr[m_userNum]->setName(userName);

	m_nodeUserName->addChild(m_labelNameArr[m_userNum]);

	m_userNum++;
}