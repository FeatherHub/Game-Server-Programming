#include "cocos2d.h"
USING_NS_CC;

#include "ui/UIText.h"

class UIChatOutput : public Node
{
public:
	CREATE_FUNC(UIChatOutput);
	virtual bool init() override;
	void DisplayMsg(const char* msg);
private:
	enum Config : int
	{ 
		MAX_CHAT_LINE_DISPLAY = 5,
		LINE_HEIGHT_DELTA = 19,
		CHAT_FONT_SIZE = 15,
	};

	ui::Text* m_chatMsgArr[MAX_CHAT_LINE_DISPLAY];
	int m_curMsgLineNum = 0;
};