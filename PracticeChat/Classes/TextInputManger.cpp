#include "TextInputManger.h"

bool TextInputManger::init()
{
	if (Node::init() == false)
	{
		return false;
	}

	// create a keyboard event listener
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_1(TextInputManger::OnKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void TextInputManger::OnKeyPressed(EventKeyboard::KeyCode keyCode)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_BACKSPACE:
		DeleteBackChar();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_A:
		m_Data.append("A");
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_B:
		m_Data.append("B");
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_C:
		m_Data.append("C");
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_D:
		m_Data.append("D");
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_E:
		m_Data.append("E");
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_F:
		m_Data.append("F");
		break;
	}
}

void TextInputManger::DeleteBackChar()
{
	if (m_Data.size() <= 0)
	{
		return;
	}

	m_Data.pop_back();
}
