#pragma once

#include "..\Packetid.h"

class BodySizeManager
{
public:
	BodySizeManager();
	int Get(unsigned short pktId);

private:
	void Init();

private:
	short m_bodySizeArr[(int)PacketId::MAX];
};