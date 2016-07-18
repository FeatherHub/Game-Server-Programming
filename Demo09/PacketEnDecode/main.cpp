#include <iostream>
using namespace std;

#include "PacketQueue.h"

#include <memory.h>

const unsigned short PACKET_ID_LOGIN = 11;

//루프:
//1. 패킷큐에 패킷들을 쌓는다
//2. 패킷을 모두 처리한다

//패킷 읽기/쓰기 테스트 함수
//전달한 패킷에 Receive 내용을 쓴다
void ReceivePacket(PacketQueue& packet);

void Print(int* a, int* a1, int* a2)
{
	cout << a << endl;
	cout << a1 << endl;
	cout << a2 << endl;
}


int main()
{	
	PacketQueue packet1(PACKET_ID_LOGIN);
	packet1 << L"jacking75";
	packet1 << L"123qwe";
	packet1 << 25;

	ReceivePacket(packet1);

	//int* a = new int[3];
	//cout << a << endl << endl;
	//cout << a << endl << a + 1 << endl << a + 2 << endl << endl;
	//cout << a << endl << a++ << endl << a++ << endl << endl;
	//cout << a++ << endl << a++ << endl << a << endl << endl;
	//Print(a, a++, a++);
	//cout << endl;
	//cout << a << endl;

	return 0;
}


void ReceivePacket(PacketQueue& packet)
{
	switch (packet.id())
	{
		case PACKET_ID_LOGIN:
			//1.
			//Packet ID가 Login이면
			//각각 ID와 PW를 의미하는
			//wchar_t[32] 타입/크기의
			//데이터 두 개와
			//Version을 의미하는
			//int 타입/크기의
			//데이터가 있다
			//2.
			//약속된 순서대로 읽는다
		{
			wchar_t UserID[32] = { 0, };
			wchar_t UserPW[32] = { 0, };
			int version = 0;

			//패킷 쓰기
			packet >> UserID;
			packet >> UserPW;
			packet >> version;
			
			//결과 출력
			printf("UserID:%S, UserPW:%S, Version:%d\n", UserID, UserPW, version);
		}
		break;
	}
}