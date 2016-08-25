#include "LobbyScene.h"
#include "Constants.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

void LobbyScene::LoginNtf(char* pData)
{
	char newbieName[MAX_USER_NAME_LEN+1];

	ProcessManager::GetInstance()->LoginNtf(pData, newbieName);

	m_userNum++;
	AddUserNameList(newbieName);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	int curUserNum = 0;
	ProcessManager::GetInstance()->LobbyUserNameList(pData, (char*)m_userNameArr, &curUserNum);
	
	m_userNum = curUserNum;
	for (int i = 0; i < m_userNum; i++)
	{
		AddUserNameList(m_userNameArr[i]);
	}
}

void LobbyScene::LobbyChatRes(char* pData)
{

}

void LobbyScene::LobbyChatNtf(char* pData)
{

}

void LobbyScene::AddUserNameList(const char* userName)
{
	Label* label = Label::create(userName, Constants::DEFAULT_FONT, Constants::USER_NAME_FONT_SIZE);
	m_nodeUserName->addChild(label);

	label->setColor(Color3B::WHITE);
	label->setPosition(Point(0, -(m_userNum * Constants::USER_NAME_POS_DELTA)));
}