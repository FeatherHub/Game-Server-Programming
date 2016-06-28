#pragma once

#include <cocos2d.h>

USING_NS_CC;

class TextInputManger;

class MainScene : Layer
{
public:
	MainScene() = default;
	~MainScene() = default;
	CREATE_FUNC(MainScene);
	static Scene* createScene();
	bool init() override;

private:
	TextInputManger* m_TextInputManager;

};