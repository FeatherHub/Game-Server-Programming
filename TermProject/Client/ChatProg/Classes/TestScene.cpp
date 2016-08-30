#include "TestScene.h"
#include "UIChatOutput.h"

#include "Constants.h"

Scene* TestScene::createScene()
{
    auto scene = Scene::create();
	auto layer = TestScene::create();
    scene->addChild(layer);

    return scene;
}

bool TestScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_a = UIChatOutput::create();
	m_a->setPosition(Point(100, 100));
	addChild(m_a);

	scheduleUpdate();

    return true;
}

void TestScene::update(float delta)
{
	static float elapsedTime = 0.0f;
	elapsedTime += delta;

	if (elapsedTime > 1.0f)
	{
		m_a->DisplayMsg(std::to_string(elapsedTime).c_str());

		elapsedTime = 0.f;
	}
}
