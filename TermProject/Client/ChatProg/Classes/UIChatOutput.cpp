#include "UIChatOutput.h"
#include "Constants.h"

bool UIChatOutPut::init()
{
	if (Node::init() == false)
	{
		return false;
	}

	setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

	for (int i = 0; i < MAX_CHAT_LINE_DISPLAY; i++)
	{
		m_chatMsgArr[i] = ui::Text::create("", Constants::DEFAULT_FONT, CHAT_FONT_SIZE);
		m_chatMsgArr[i]->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		m_chatMsgArr[i]->setPosition(
			Point(0, (MAX_CHAT_LINE_DISPLAY - i) * LINE_HEIGHT_DELTA));
		addChild(m_chatMsgArr[i]);
	}

	return true;
}

void UIChatOutPut::DisplayMsg(const char* msg)
{
	if (m_curMsgLineNum >= Config::MAX_CHAT_LINE_DISPLAY)
	{
		auto tmp = m_chatMsgArr[0];
		
		for (int i = 0; i < MAX_CHAT_LINE_DISPLAY-1; i++)
		{
			m_chatMsgArr[i] = m_chatMsgArr[i + 1];
			m_chatMsgArr[i]->setPosition(
				Point(0, (MAX_CHAT_LINE_DISPLAY-i) * LINE_HEIGHT_DELTA));
		}

		m_chatMsgArr[MAX_CHAT_LINE_DISPLAY - 1] = tmp;
		m_chatMsgArr[MAX_CHAT_LINE_DISPLAY - 1]
			->setPosition(Point(0, 1 * LINE_HEIGHT_DELTA));

		m_chatMsgArr[MAX_CHAT_LINE_DISPLAY-1]
			->setString(msg);
	}
	else
	{
		m_chatMsgArr[m_curMsgLineNum]
			->setString(msg);

		m_curMsgLineNum++;
	}
}

