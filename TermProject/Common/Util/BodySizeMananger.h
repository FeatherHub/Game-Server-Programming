#pragma once

#include "..\Packetid.h"

class BodySizeManager
{
public:
	void Init();
	int Get(unsigned short pktId);

private:
	short m_bodySizeArr[PacketId::MAX];
};