#pragma once

#include <cocos2d.h>

USING_NS_CC;

class TextInputManger : Node 
{
public:
	TextInputManger() = default;
	~TextInputManger() = default;
	CREATE_FUNC(TextInputManger);
	bool init() override;
	std::string& GetData() { return m_Data; }

private:
	void OnKeyPressed(EventKeyboard::KeyCode keyCode);
	void DeleteBackChar();

private:
	std::string m_Data;
};