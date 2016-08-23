#pragma once

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"

USING_NS_CC;

class ConnectScene : public Layer
{
public:
    static Scene* createScene();
	CREATE_FUNC(ConnectScene);
    virtual bool init() override;
private:
	void OnConnectBtnTouched(Ref *pSender, ui::Widget::TouchEventType type);
	void OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType);
private:
	ui::TextField* m_tfPort;
	ui::TextField* m_tfIP;
	ui::TextField* m_tfMsg;
	ui::Button* m_btnConnect;
};
