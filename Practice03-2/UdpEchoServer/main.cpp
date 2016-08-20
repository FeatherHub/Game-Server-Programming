#include <WinSock2.h>

//아이피 문자열<->아이피 숫자
//변환 함수 제공
#include <WS2tcpip.h>

#include <stdio.h>

#include <windows.h>

void PrintClientInfo(SOCKADDR_IN& client)
{
	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, (PVOID)&client, clientIP, sizeof(clientIP));
	printf("IP: %s Port: %d \n", clientIP, ntohs(client.sin_port));
}

//UDP Echo Server
int main()
{
	int ret = 0;

	//WSA
	WSADATA wd;
	ret = WSAStartup(MAKEWORD(2, 2), &wd);
	if (ret != 0) return 0;

	//Socket
	SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSock == INVALID_SOCKET) return 0;

	//my addr
	USHORT PORT = 23452;
	const char* IP = "172.0.0.1";
	
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	//Bind with my addr
	ret = bind(udpSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR) return 0;
	
	SOCKADDR_IN clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	const int BUFFER_SIZE = 512;
	char recvBuf[BUFFER_SIZE] = { 0, };
	char sendBuf[BUFFER_SIZE] = { 0, };
	int recvNum = 0, sendNum = 0;

	while (true)
	{
		recvNum = recvfrom(udpSock, recvBuf, BUFFER_SIZE, 0, 
					(SOCKADDR*)&clientAddr, &clientAddrSize);

		if (recvNum == SOCKET_ERROR)
		{
			printf("recvfrom error \n");
			break;
		}
		else if (recvNum == 0) 
		{
			printf("Client ends gracefully \n");
			PrintClientInfo(clientAddr);
			break; 
		}

		printf("Client sends data \n");
		PrintClientInfo(clientAddr);
		printf("Data: %s \n\n", recvBuf);

		memcpy(sendBuf, recvBuf, recvNum);

		sendNum = sendto(udpSock, sendBuf, recvNum, 0, (SOCKADDR*)&clientAddr, clientAddrSize);

		ZeroMemory(recvBuf, recvNum);
		ZeroMemory(sendBuf, sendNum);
	}
	
	closesocket(udpSock);

	WSACleanup();

	return 0;
}