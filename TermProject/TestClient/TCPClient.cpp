#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "..\Server\Network\NetPacket.h"
#include "..\Common\Packet.h"
#include "..\Common\PacketId.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 23452
#define BUFSIZE    512

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) // success
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, SERVERIP, (void *)&serveraddr.sin_addr.s_addr);

	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
	}

	//내용물
	TestReqPkt reqPkt;
	reqPkt.num = 10;

	//포장
	NNetworkLib::Packet pkt;
	pkt.id = PacketId::TestReq;
	//pkt.pData = new char[bodySize];
	//CopyMemory(pkt.pData, &reqPkt, bodySize);

	char sendBuff[512] = { 0, };
	CopyMemory(sendBuff, &pkt.id, sizeof(pkt.id));
	CopyMemory(sendBuff + sizeof(pkt.id), &reqPkt, sizeof(TestReqPkt));

	int bodySize = sizeof(TestReqPkt);
	send(sock, sendBuff, PACKET_HEADER_SIZE + bodySize, 0);
	printf("send p id %d size %d data %d \n", pkt.id, bodySize, reqPkt.num);

	char recvBuf[512] = { 0, };
	int readPos = 0;
	int recvNum = 0;

	while (true)
	{
		//받기
		int recvedNum = recv(sock, recvBuf + readPos, 512 - recvNum, 0);
		if (recvedNum < 0)
		{
			printf("error \n");
			break;
		}

		printf("recv %d \n", recvedNum);

		recvNum += recvedNum;

		while (recvNum > PACKET_HEADER_SIZE)
		{
			//패킷화
			NNetworkLib::Packet pkt;
			CopyMemory(&pkt, recvBuf + readPos, sizeof(pkt));

			int bodySize = sizeof(TestResPkt);
			if (recvNum < PACKET_HEADER_SIZE + bodySize)
			{
				break;
			}

			//버퍼 상태 갱신
			readPos += PACKET_HEADER_SIZE + bodySize;
			recvNum -= PACKET_HEADER_SIZE + bodySize;

			//데이터 추출
			TestResPkt* resPkt = (TestResPkt*)pkt.pData;

			//출력
			printf("%d \n", resPkt->num);

			if (resPkt->num == 0)
			{
				printf("test success \n");
				return 0;
			}

			//앞의 데이터는 사용했으니 버퍼 당기기
			CopyMemory(recvBuf, recvBuf + readPos, recvNum);
			readPos = 0;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}