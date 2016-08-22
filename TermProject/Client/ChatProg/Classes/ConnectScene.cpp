#include "ConnectScene.h"

/*static*/ Scene* ConnectScene::createScene()
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

	m_tfIP = ui::TextField::create("*ip*", "fonts/Marker Felt.ttf", 24);
	m_tfIP->setPosition(Point(100, 250));
	m_tfIP->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfIP);

	m_tfPort = ui::TextField::create("*Port number*", "fonts/Marker Felt.ttf", 24);
	m_tfPort->setPosition(Point(150, 200));
	m_tfPort->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfPort);

	m_tfMsg = ui::TextField::create("", "fonts/Marker Felt.ttf", 24);
	m_tfMsg->setAttachWithIME(false);
	m_tfMsg->setPosition(Point(150, 150));

	m_btnConnect = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnConnect->setPosition(Point(150, 100));
	m_btnConnect->addTouchEventListener(CC_CALLBACK_2(ConnectScene::OnConnectBtnTouched, this));
	
	addChild(m_btnConnect);

    return true;
}

void ConnectScene::OnConnectBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{

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

