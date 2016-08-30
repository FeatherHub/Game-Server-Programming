#pragma once

#include "cocos2d.h"

USING_NS_CC;

class UIChatOutPut;

class TestScene : public Layer
{
public:
    static Scene* createScene();
	CREATE_FUNC(TestScene);
    virtual bool init() override;

private:
	virtual void update(float delta) override;

	UIChatOutPut* m_a;
};
