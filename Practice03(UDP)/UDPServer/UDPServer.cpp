#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

const int SERVER_PORT = 9000;
const int BUFFER_SIZE = 512;

int main()
{
	//Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Socket
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	//Bind
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);

	bind(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	char buf[BUFFER_SIZE + 1] = { 0, };
	int recvLen = 0;
	while (true)
	{
		//ReceiveFrom
		recvLen = recvfrom(sock, buf, BUFFER_SIZE, 0, (SOCKADDR*)&clientAddr, &addrLen);

		buf[recvLen] = '\0';
		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
		printf("[UDP/%s:%d %s \n", clientIP, ntohs(clientAddr.sin_port), buf);

		//SendTo
		sendto(sock, buf, recvLen, 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
	}

	//Close socket
	closesocket(sock);

	//Clean winsock
	WSACleanup();

	return 0;
}