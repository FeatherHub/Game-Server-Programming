#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

const int SERVER_PORT = 9000;
const int BUFFER_SIZE = 512;
const char* SERVER_IP = "127.0.0.1";

int main()
{
	//Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Sock
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	//Set addr member
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, (void*)&serverAddr.sin_addr.s_addr);

	//Network datas
	SOCKADDR_IN peerAddr;
	int addrLen = sizeof(peerAddr);
	char buf[BUFFER_SIZE + 1] = { 0, };
	int dataLen = 0;

	while (true)
	{
		fgets(buf, BUFFER_SIZE + 1, stdin);
		dataLen = strlen(buf);
		buf[dataLen - 1] = '\0';

		//SendTo
		dataLen = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		//RecvFrom
		dataLen = recvfrom(sock, buf, BUFFER_SIZE, 0, (SOCKADDR*)&peerAddr, &addrLen);

		//Check IP of sender
		if (memcmp(&peerAddr, &serverAddr, sizeof(peerAddr)))
		{
			printf("Wrong data \n");
			continue;
		}

		buf[dataLen] = '\0';
		printf("받은 데이터 %s \n", buf);
	}

	//Close socket
	closesocket(sock);

	//Clean winsock
	WSACleanup();
	return 0;
}