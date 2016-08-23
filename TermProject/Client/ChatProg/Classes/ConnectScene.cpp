#include "ConnectScene.h"
#include "LoginScene.h"

#include "Constants.h"

#include "..\ClientLogic\RequestManager.h"

Scene* ConnectScene::createScene()
{
    auto scene = Scene::create();
	auto layer = ConnectScene::create();
    scene->addChild(layer);

    return scene;
}

bool ConnectScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto winSizeHalf = Director::getInstance()->getWinSize().width / 2;

	m_tfMsg = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfMsg->setPosition(Point(winSizeHalf, 250));
	m_tfMsg->setString("Connect Scene");
	m_tfMsg->setEnabled(false);

	addChild(m_tfMsg);

	m_tfIP = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfIP->setString("127.0.0.1");
	m_tfIP->setPosition(Point(winSizeHalf, 200));
	m_tfIP->setColor(Color3B(100, 100, 100));
	m_tfIP->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfIP);

	m_tfPort = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfPort->setString("23452");
	m_tfPort->setPosition(Point(winSizeHalf, 150));
	m_tfPort->setColor(Color3B(100, 100, 100));
	m_tfPort->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfPort);

	m_btnConnect = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnConnect->setPosition(Point(winSizeHalf, 80));
	m_btnConnect->addTouchEventListener(CC_CALLBACK_2(ConnectScene::OnConnectBtnTouched, this));
	
	addChild(m_btnConnect);

    return true;
}

void ConnectScene::OnConnectBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		m_tfMsg->setString("Wait...");

		auto& port = m_tfPort->getString();
		auto& ip = m_tfIP->getString();

		auto res = RequestManager::GetInstance()->RequestConnect(port, ip);
		
		switch (res)
		{
		case CONNECT_REQ_OK:
			Director::getInstance()->replaceScene(LoginScene::createScene());
			break;
		case CONNECT_REQ_FAIL:
			m_tfMsg->stopAllActions();
			m_tfMsg->setString("Connect Failed");
			m_tfMsg->runAction(Blink::create(0.3f, 1));
			break;
		}
	}
}

void ConnectScene::OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType)
{
	auto textField = (ui::TextField*)pSender;

	switch (eventType)
	{
	case ui::TextField::EventType::ATTACH_WITH_IME:
		textField->setColor(Color3B::WHITE);
		break;
	case ui::TextField::EventType::DETACH_WITH_IME:
		textField->setColor(Color3B(100, 100, 100));
		break;
	}
}

