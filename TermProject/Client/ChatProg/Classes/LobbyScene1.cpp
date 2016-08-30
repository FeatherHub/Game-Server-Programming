#include "LobbyScene.h"
#include "Client.h"
#include "Constants.h"
#include "UIChatOutput.h"
#include "ui/UIText.h"

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
	m_tfMsg->setPosition(Point(winWidthHalf, winHeight*0.855f));
	m_tfMsg->setString("Lobby Scene");
	m_tfMsg->setEnabled(false);
	addChild(m_tfMsg);

	m_tfChatInput = ui::TextField::create("-", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	m_tfChatInput->attachWithIME();
	m_tfChatInput->setPosition(Point(winWidthHalf*0.4f, 30));
	m_tfChatInput->setCursorChar('|');
	m_tfChatInput->setCursorEnabled(true);
	m_tfChatInput->setMaxLength(MAX_CHAT_MSG_LEN);
	m_tfChatInput->setMaxLengthEnabled(true);
	m_tfChatInput->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_tfChatInput->addEventListener(
		[&](Ref* pSender, ui::TextField::EventType eventType)
	{
		OnTextFieldEvent(pSender, eventType);
	});

	addChild(m_tfChatInput);

	ui::Text* You = ui::Text::create(
		"You: ", Constants::DEFAULT_FONT, Constants::DEFAULT_FONT_SIZE);
	You->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	You->setPosition(Point(-30, 0));
	m_tfChatInput->addChild(You);

	m_uiChatOutput = UIChatOutput::create();
	m_uiChatOutput->setPosition(Point(10, winWidthHalf * 0.5f));
	addChild(m_uiChatOutput);

	m_btnSendChat = ui::Button::create("ButtonSelect.png", "ButtonSelected.png");
	m_btnSendChat->setPosition(Point(winWidthHalf*1.5, 50));
	m_btnSendChat->addTouchEventListener(CC_CALLBACK_2(LobbyScene::OnChatSendBtnTouched, this));
	addChild(m_btnSendChat);

	m_nodeUserName = Node::create();
	m_nodeUserName->setPosition(Point(winWidthHalf*1.7f, winHeight*0.75f));
	addChild(m_nodeUserName);

	//Register KeyboardListener
	auto* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = 
		CC_CALLBACK_2(LobbyScene::OnKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	
	//Network
	m_pRefNetwork = Network::GetInstance();

	//Member
	m_userNum = 0;

	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		m_userPool[i] = new Client();
	}
	
	//Request user name list once at init
	RequestManager::GetInstance()->RequestUserNameList();

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
			LobbyChatRes(pkt.pDataAtBuff);
			break;
		case PacketId::LobbyChatNtf:
			LobbyChatNtf(pkt.pDataAtBuff);
			break;
		case PacketId::RemoveUserNtf:
			RemoveUserNtf(pkt.pDataAtBuff);
			break;
		}
	}
}

void LobbyScene::ReqSendMsg()
{
	std::string szMsgSet = "You: ";
	const std::string szMsg = m_tfChatInput->getString();

	szMsgSet.append(szMsg);

	auto res = RequestManager::GetInstance()->RequestLobbyChat(szMsg);
	switch (res)
	{
	case LOBBY_CHAT_REQ_OK:
		m_tfMsg->setString("Message sent!");
		m_uiChatOutput->DisplayMsg(szMsgSet.c_str());
		m_tfChatInput->setString("");
		break;
	case LOBBY_CHAT_REQ_NO_MSG:
		m_tfMsg->setString("Message is empty");
		break;
	case LOBBY_CHAT_REQ_OVERFLOW_MSG:
		m_tfMsg->setString("Message is overflowed");
		break;
	}
}

void LobbyScene::OnTextFieldEvent(Ref* pSender, ui::TextField::EventType eventType)
{
	switch (eventType)
	{
	case cocos2d::ui::TextField::EventType::DETACH_WITH_IME:
		m_tfChatInput->setInsertText(true);
		break;
	}
}

void LobbyScene::OnChatSendBtnTouched(Ref *pSender, ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		ReqSendMsg();
		break;
	}
}

void LobbyScene::OnKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
		ReqSendMsg();
		break;
	}
}

LobbyScene::~LobbyScene()
{
	for (int i = 0; i < MAX_LOBBY_USER_NUM; i++)
	{
		if(m_userPool[i] != nullptr)
			delete m_userPool[i];
	}
}
