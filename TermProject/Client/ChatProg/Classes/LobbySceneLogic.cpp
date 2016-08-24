#include "LobbyScene.h"
#include "Constants.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

void LobbyScene::LoginNtf(char* pData)
{
	char newbieName[MAX_USER_NAME_LEN];

	ProcessManager::GetInstance()->LoginNtf(pData, newbieName);

	AddUserToNode(newbieName);
}

void LobbyScene::LobbyUserNameListRes(char* pData)
{
	ProcessManager::GetInstance()->LobbyUserNameList(nullptr, nullptr);
}

void LobbyScene::LobbyChatRes(char* pData)
{

}

void LobbyScene::LobbyChatNtf(char* pData)
{

}

void LobbyScene::AddUserToNode(char* newbieName)
{

}