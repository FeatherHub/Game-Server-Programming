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
	ProcessManager::GetInstance()->LoginNtf(pData, m_userNameArr[m_userNum]);

	AddUserNameToList(m_userNameArr[m_userNum]);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	int userNum = 0;
	ProcessManager::GetInstance()->LobbyUserNameList(pData, (char*)m_userNameArr, &userNum);

	for (int i = 0; i < userNum; i++)
	{
		AddUserNameToList(m_userNameArr[i]);
	}
}

void LobbyScene::LobbyChatRes(char* pData)
{

}

void LobbyScene::LobbyChatNtf(char* pData)
{

}

void LobbyScene::AddUserNameToList(const char* userName)
{
	m_userNum++;

	Label* label = Label::create(userName, Constants::DEFAULT_FONT, Constants::USER_NAME_FONT_SIZE);
	label->setPosition(Point(0, -(m_userNum * Constants::USER_NAME_POS_DELTA)));

	m_nodeUserName->addChild(label);
}