#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"

USING_NS_CC;

class ConnectScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
	CREATE_FUNC(ConnectScene);
    virtual bool init() override;
private:
	void OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType);
	void OnConnectBtnTouched(Ref *pSender, ui::Widget::TouchEventType type);
private:

private:
	ui::TextField* m_tfPort;
	ui::TextField* m_tfIP;
	ui::TextField* m_tfMsg;
	ui::Button* m_btnConnect;
};
