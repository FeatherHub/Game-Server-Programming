#pragma once

#include "..\Packetid.h"

class BodySizeManager
{
public:
	BodySizeManager();
	void Init();
	int Get(unsigned short pktId);

private:
	short m_bodySizeArr[(int)PacketId::MAX];
};