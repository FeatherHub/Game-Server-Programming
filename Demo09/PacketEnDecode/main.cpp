#include <iostream>

#include "Packet.h"

const unsigned short PACKET_ID_LOGIN = 11;

void ReceivePacket(Packet& packet);

int main()
{	
	Packet packet1(PACKET_ID_LOGIN);
	packet1 << L"jacking75";
	packet1 << L"123qwe";
	packet1 << 225;

	ReceivePacket(packet1);

	return 0;
}


void ReceivePacket(Packet& packet)
{
	switch (packet.id())
	{
		case PACKET_ID_LOGIN:
		{
			wchar_t UserID[32] = { 0, };
			wchar_t UserPW[32] = { 0, };
			int version = 0;

			packet >> UserID;
			packet >> UserPW;
			packet >> version;

			printf("UserID:%S, UserPW:%S, Version:%d\n", UserID, UserPW, version);
		}
		break;
	}
}