#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"

USING_NS_CC;

#include "..\..\Common\Constants.h"

class UIChatOutput;

class Network;
class Client;

class LobbyScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(LobbyScene);
	virtual ~LobbyScene();
	virtual bool init() override;
private:
	void update(float delta);
	void ReqSendMsg();
	void OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType);
	void OnChatSendBtnTouched(Ref *pSender, ui::Widget::TouchEventType type);
	void OnKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
private:
	//////////////////////////////////////////////////
	// LobbyScene2.cpp
	//////////////////////////////////////////////////

	void LoginNtf(char* pData);
	void LobbyUserNameListRes(char* pData);
	void LobbyChatRes(char* pData);
	void LobbyChatNtf(char* pData);
	void RemoveUserNtf(char* pData);

	void AddUserNameToListUI(Client* user);
private:
	ui::TextField* m_tfMsg;
	ui::TextField* m_tfChatInput;
	ui::Button* m_btnSendChat;
	UIChatOutput* m_uiChatOutput;
	Node* m_nodeUserName;
	Label* m_labelNameArr[MAX_LOBBY_USER_NUM];

	Network* m_pRefNetwork;
	Client* m_userPool[MAX_LOBBY_USER_NUM];
	int m_userNum = 0; 
};
