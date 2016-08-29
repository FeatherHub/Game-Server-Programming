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
	ProcessManager::GetInstance()->LoginNtf(pData, m_userPool[m_userNum]);

	AddUserNameToList(TODO);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	int userListNum = -1;
	ProcessManager::GetInstance()->LobbyUserNameList(pData, m_userPool, &userListNum);

	for (int i = 0; i < userListNum; i++)
	{
		AddUserNameToList(TODO);
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

	for (int targetIdx = 0; targetIdx < m_userNum; targetIdx++)
	{
		if (ntfPkt->clientIdx == m_labelNameArr[targetIdx]->getTag())
		{
			if (targetIdx != m_userNum - 1) //마지막 원소가 아니면
			{
				//update ui position
				m_labelNameArr[m_userNum - 1]->setPosition(Point(0, -((targetIdx + 1) * Constants::USER_NAME_POS_DELTA)));

				//swap data
				auto* tmp = m_labelNameArr[targetIdx];
				m_labelNameArr[targetIdx] = m_labelNameArr[m_userNum - 1];
				m_labelNameArr[m_userNum - 1] = tmp;

				//update index
				targetIdx = m_userNum - 1;
			}

			//remove ui
			auto* childUi = m_labelNameArr[targetIdx];
			m_nodeUserName->removeChild(childUi, true);

			m_userNum--;

			break;
		}
	}
}

void LobbyScene::AddUserNameToList(User* user)
{
	m_labelNameArr[m_userNum] = Label::create(userName, Constants::DEFAULT_FONT, Constants::USER_NAME_FONT_SIZE);
	m_labelNameArr[m_userNum]->setPosition(Point(0, -((m_userNum+1) * Constants::USER_NAME_POS_DELTA)));
	m_labelNameArr[m_userNum]->setTag(clientIdx);

	m_nodeUserName->addChild(m_labelNameArr[m_userNum]);

	m_userNum++;
}