#include "LobbyScene.h"
#include "User.h"
#include "Constants.h"

#include "..\Network\Network.h"
#include "..\..\Common\Packetid.h"
#include "..\ClientLogic\RequestManager.h"
#include "..\ClientLogic\ProcessManager.h"

Scene* LobbyScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LobbyScene::create();
	scene->addChild(layer);

	return scene;
}

bool LobbyScene::init()
{
	auto winHeight = Director::getInstance()->getWinSize().height;
	auto winWidthHalf = Director::getInstance()->getWinSize().width / 2;

	m_tfMsg = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfMsg->setPosition(Point(winWidthHalf, 250));
	m_tfMsg->setString("Lobby Scene");
	m_tfMsg->setEnabled(false);

	addChild(m_tfMsg);

	m_tfID = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfID->setPosition(Point(winWidthHalf, 200));
	m_tfID->setColor(Color3B(100, 100, 100));
	m_tfID->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfID);

	m_tfPW = ui::TextField::create("", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfPW->setPasswordEnabled(true);
	m_tfPW->setPasswordStyleText("*");
	m_tfPW->setPosition(Point(winWidthHalf, 150));
	m_tfPW->setColor(Color3B(100, 100, 100));
	m_tfPW->addEventListener([&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfPW);

	m_btnLogin = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnLogin->setPosition(Point(winWidthHalf, 80));
	m_btnLogin->addTouchEventListener(CC_CALLBACK_2(LobbyScene::OnLoginBtnTouched, this));

	addChild(m_btnLogin);

	m_nodeUserName = Node::create();
	m_nodeUserName->setPosition(Point(winWidthHalf*1.5f, winHeight));

	addChild(m_nodeUserName);

	m_pRefNetwork = Network::GetInstance();

	m_userNum = 0;

	/* Request user name list once at init*/
	RequestManager::GetInstance()->RequestUserNameList();

	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		m_userPool[i] = new User();
	}

	scheduleUpdate();

	return true;
}

void LobbyScene::update(float delta)
{
	m_pRefNetwork->Run();

	while (m_pRefNetwork->PktQueueEmpty() == false)
	{
		auto pkt = m_pRefNetwork->GetPacket();
		switch (pkt.id)
		{
		case PacketId::LoginNtf:
			LoginNtf(pkt.pDataAtBuff);
			break;
		case PacketId::LobbyUserNameListRes:
			LobbyUserNameListRes(pkt.pDataAtBuff);
			break;
		case PacketId::LobbyChatRes:

			break;
		case PacketId::LobbyChatNtf:

			break;
		case PacketId::RemoveUserNtf:
			RemoveUserNtf(pkt.pDataAtBuff);
			break;
		}
	}
}

void LobbyScene::OnLoginBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{
	//switch (type)
	//{
	//case cocos2d::ui::Widget::TouchEventType::BEGAN:
	//	auto& id = m_tfID->getString();
	//	auto& pw = m_tfPW->getString();

	//	auto res = RequestManager::GetInstance()->RequestLogin(id, pw);
	//	switch (res)
	//	{
	//	case LOGIN_REQ_OK:
	//		break;
	//	case LOGIN_REQ_ID_OR_PW_EMPTY:
	//		m_tfMsg->setString("ID or PW is empty");
	//		break;
	//	}
	//}
}

void LobbyScene::OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType)
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

LobbyScene::~LobbyScene()
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		delete m_userPool[i];
	}
}
