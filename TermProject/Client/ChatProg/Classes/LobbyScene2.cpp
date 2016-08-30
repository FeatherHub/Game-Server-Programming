#include "LobbyScene.h"
#include "UIChatOutput.h"
#include "Constants.h"
#include "Client.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

////////////////////////////////////////////////////////////////
// ProcessManager와 연동하여 Scene의 데이터와 UI를 갱신한다.
////////////////////////////////////////////////////////////////

void LobbyScene::LoginNtf(char* pData)
{
	auto* resPkt = (LoginNtfPkt*)pData;
	
	ProcessManager::GetInstance()->LoginNtf(pData, m_userPool[resPkt->newbieClientIdx]);

	AddUserNameToListUI(m_userPool[resPkt->newbieClientIdx]);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	int userListNum = -1;
	ProcessManager::GetInstance()->LobbyUserNameList(pData, m_userPool, &userListNum);

	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		if(m_userPool[i]->Connected())
		{
			AddUserNameToListUI(m_userPool[i]);
		}
	}
}

void LobbyScene::LobbyChatRes(char* pData)
{
	LobbyChatResPkt* resPkt = (LobbyChatResPkt*)pData;
	
	if (resPkt->permitted)
	{
		m_tfMsg->setString("Your message has been permitted");
	}
	else
	{
		m_tfMsg->setString("Your message has been denied");
	}
}

void LobbyScene::LobbyChatNtf(char* pData)
{
	LobbyChatNtfPkt* ntfPkt = (LobbyChatNtfPkt*)pData;

	std::string& senderId = m_userPool[ntfPkt->senderIdx]->GetName();
	auto msg = ntfPkt->msg;

	senderId += ": ";
	senderId += msg;

	m_uiChatOutput->DisplayMsg(senderId.c_str());
}

void LobbyScene::RemoveUserNtf(char* pData)
{
	RemoveUserNtfPkt* ntfPkt = (RemoveUserNtfPkt*)pData;

	Client* userToRemove = m_userPool[ntfPkt->clientIdx];
	if (userToRemove == nullptr)
	{
		m_tfMsg->setString("RemoveUserNtf nullptr");
		return;
	}

	//Update Data
	userToRemove->Disconnect();

	//Update UI
	for (int idx = 0; idx < m_userNum; idx++)
	{
		if (m_labelNameArr[idx]->getTag() == userToRemove->GetKey())
		{
			if (idx != m_userNum - 1) //마지막 원소가 아니면
			{
				//update ui position
				m_labelNameArr[m_userNum - 1]->
					setPosition(Point(0, -((idx + 1) * Constants::USER_NAME_POS_DELTA)));

				//swap data
				auto* tmp = m_labelNameArr[idx];
				m_labelNameArr[idx] = m_labelNameArr[m_userNum - 1];
				m_labelNameArr[m_userNum - 1] = tmp;

				//update index
				idx = m_userNum - 1;
			}

			//remove ui
			auto* childUi = m_labelNameArr[idx];
			childUi->removeFromParentAndCleanup(true);

			m_userNum--;

			break;
		}
	}
}

void LobbyScene::AddUserNameToListUI(Client* user)
{
	//Add to UI
	m_labelNameArr[m_userNum] = Label::create(user->GetName(), Constants::DEFAULT_FONT, Constants::USER_NAME_FONT_SIZE);
	m_labelNameArr[m_userNum]->setPosition(Point(0, -((m_userNum+1) * Constants::USER_NAME_POS_DELTA)));
	m_labelNameArr[m_userNum]->setTag(user->GetKey());

	m_nodeUserName->addChild(m_labelNameArr[m_userNum]);

	m_userNum++;
}