#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"
USING_NS_CC;

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
	ui::TextField* m_tfID;
	ui::TextField* m_tfPW;
	ui::TextField* m_tfMsg;
	ui::Button* m_btnLogin;

	Network* m_RefNetwork;
};