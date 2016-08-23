#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"
USING_NS_CC;

class LoginScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(LoginScene);
	virtual bool init() override;
private:
	void OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType);
	void OnLoginBtnTouched(Ref *pSender, ui::Widget::TouchEventType type);
private:
	ui::TextField* m_tfId;
	ui::TextField* m_tfPw;
	ui::TextField* m_tfMsg;
	ui::Button* m_btnLogin;
};