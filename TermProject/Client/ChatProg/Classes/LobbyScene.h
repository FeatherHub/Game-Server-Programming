#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"
USING_NS_CC;

#include "..\..\Common\Constants.h"

class Network;

class LobbyScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(LobbyScene);
	virtual bool init() override;
private:
	void update(float delta);
	void OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType);
	void OnLoginBtnTouched(Ref *pSender, ui::Widget::TouchEventType type);
private:
	//////////////////////////////////////////////////
	// LobbySceneReflect.cpp
	//////////////////////////////////////////////////
	void LoginNtf(char* pData);
	void LobbyUserNameListRes(char* pData);
	void LobbyChatRes(char* pData);
	void LobbyChatNtf(char* pData);

	void AddUserNameToList(const char* userName);

private:
	//ui
	ui::TextField* m_tfID;
	ui::TextField* m_tfPW;
	ui::TextField* m_tfMsg;
	ui::Button* m_btnLogin;
	
	Node* m_nodeUserName;
	char m_userNameList[MAX_LOBBY_USER_NUM][MAX_USER_NAME_LEN+1];
	int m_userNum = 0;
	
	Network* m_pRefNetwork;
};