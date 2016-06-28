#include "MainScene.h"
#include "TextInputManger.h"

Scene* MainScene::createScene()
{
	auto scene = Scene::create();

	auto layer = MainScene::create();

	scene->addChild(layer);

	return scene;
}

bool MainScene::init()
{
	if (Layer::init() == false)
	{
		return false;
	}

	m_TextInputManager = TextInputManger::create();

	return true;
}
