#include "LoginScene.h"
#include "LobbyScene.h"

#include "Constants.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

Scene* LoginScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoginScene::create();
	scene->addChild(layer);

	return scene;
}

bool LoginScene::init()
{
	auto winWidthHalf = Director::getInstance()->getWinSize().width / 2;

	m_tfMsg = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfMsg->setPosition(Point(winWidthHalf, 250));
	m_tfMsg->setString("Login Scene");
	m_tfMsg->setEnabled(false);

	addChild(m_tfMsg);

	m_tfID = ui::TextField::create("*Your ID*", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfID->setPosition(Point(winWidthHalf, 200));
	m_tfID->setColor(Color3B(100, 100, 100));
	m_tfID->setMaxLengthEnabled(true);
	m_tfID->setMaxLength(MAX_USER_NAME_LEN);
	m_tfID->setCursorChar('|');
	m_tfID->setCursorEnabled(true);
	m_tfID->attachWithIME();
	m_tfID->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfID);

	m_tfPW = ui::TextField::create("*Your Password*", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfPW->setPasswordEnabled(true);
	m_tfPW->setPasswordStyleText("*");
	m_tfPW->setPosition(Point(winWidthHalf, 150));
	m_tfPW->setColor(Color3B(100, 100, 100));
	m_tfPW->setMaxLengthEnabled(true);
	m_tfPW->setMaxLength(MAX_USER_PW_LEN);
	m_tfPW->setCursorChar('|');
	m_tfPW->setCursorEnabled(true);
	m_tfPW->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfPW);

	m_btnLogin = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnLogin->setPosition(Point(winWidthHalf, 80));
	m_btnLogin->addTouchEventListener(CC_CALLBACK_2(LoginScene::OnLoginBtnTouched, this));

	addChild(m_btnLogin);

	m_RefNetwork = Network::GetInstance();

	return true;
}

void LoginScene::update(float delta)
{
	//이후에 delta 값을 이용하여 
	//네트워크 실행 횟수를 최적화한다.

	m_RefNetwork->Run();

	while (m_RefNetwork->PktQueueEmpty() == false)
	{
		auto pkt = m_RefNetwork->GetPacket();
		switch (pkt.id)
		{
		case PacketId::LoginRes:
			auto res = ProcessManager::GetInstance()->LoginRes(pkt.pDataAtBuff);
			switch (res)
			{
			case LOGIN_RES_OK:
				unscheduleUpdate();
				Director::getInstance()->replaceScene(LobbyScene::createScene());
				break;
			case LOGIN_RES_NO:
				m_tfMsg->setString("Login failed");
				break;
			}
		}
	}
}

void LoginScene::OnLoginBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		auto& id = m_tfID->getString();
		auto& pw = m_tfPW->getString();

		auto res = RequestManager::GetInstance()->RequestLogin(id, pw);
		switch (res)
		{
		case LOGIN_REQ_OK:
			scheduleUpdate();
			break;
		case LOGIN_REQ_ID_OR_PW_EMPTY:
			m_tfMsg->setString("ID or PW is empty");
			break;
		case LOGIN_REQ_ID_OR_PW_OVERFLOW:
			m_tfMsg->setString("ID or PW is overflowed");
			break;
		}
	}
}

void LoginScene::OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType)
{
	auto textField = (ui::TextField*)pSender;

	switch (eventType)
	{
	case ui::TextField::EventType::ATTACH_WITH_IME:
		textField->setColor(Color3B::WHITE);
		break;
	case ui::TextField::EventType::DETACH_WITH_IME:
		textField->setColor(Color3B(100, 100, 100));
		textField->setInsertText(true);
		break;
	}
}