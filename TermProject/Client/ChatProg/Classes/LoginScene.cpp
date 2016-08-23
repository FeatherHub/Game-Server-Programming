#include "LoginScene.h"

#include "..\ClientLogic\RequestManager.h"

Scene* LoginScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoginScene::create();
	scene->addChild(layer);

	return scene;
}

bool LoginScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_tfId = ui::TextField::create("*ip*", "fonts/Marker Felt.ttf", 24);
	m_tfId->setPosition(Point(100, 250));
	m_tfId->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfId);

	m_tfPw = ui::TextField::create("*Port number*", "fonts/Marker Felt.ttf", 24);
	m_tfPw->setPosition(Point(150, 200));
	m_tfPw->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfPw);

	m_tfMsg = ui::TextField::create("", "fonts/Marker Felt.ttf", 24);
	m_tfMsg->setAttachWithIME(false);
	m_tfMsg->setPosition(Point(150, 150));

	addChild(m_tfMsg);

	m_btnLogin = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnLogin->setPosition(Point(150, 100));
	m_btnLogin->addTouchEventListener(CC_CALLBACK_2(LoginScene::OnLoginBtnTouched, this));

	addChild(m_btnLogin);

	return true;
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
		break;
	}
}

void LoginScene::OnLoginBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{

//	auto res = RequestManager::GetInstance()->RequestLogin();

}

