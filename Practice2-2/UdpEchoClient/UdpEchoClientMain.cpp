#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>

#include <stdio.h>

int main()
{
	int ret = 0;

	WSADATA wsa;
	ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret != 0) { system("color 0f"); return 0; }

	SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSock == INVALID_SOCKET) { system("color 1f");  return 0; }

	//to addr
	char* serverIP = "127.0.0.1";
	USHORT serverPort = 23452;

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	InetPton(AF_INET, serverIP, (PVOID)&serverAddr.sin_addr);

	SOCKADDR_IN peerAddr;
	ZeroMemory(&peerAddr, sizeof(peerAddr));
	int peerAddrSize = sizeof(peerAddr);

	const int BUFFER_SIZE = 512;
	char buff[BUFFER_SIZE] = { 0, };
	int len = 0;
	while (true)
	{
		if (fgets(buff, BUFFER_SIZE, stdin) == nullptr)
		{
			printf("fgets error \n");
			break;
		}

		len = strlen(buff);
		len++; //'\0'를 위해

		ret = sendto(udpSock, buff, len, 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		ret = recvfrom(udpSock, buff, BUFFER_SIZE, 0, (SOCKADDR*)&peerAddr, &peerAddrSize);

		//받았는데 아까 보낸 곳으로부터 온 것이 아니면 그에 대한 처리를 한다
		if (memcmp(&serverAddr, &peerAddr, peerAddrSize) != 0)
		{
			printf("who are you? \n");
			continue;
		}

		printf("recvfrom data : %s \n", buff);
	}

	closesocket(udpSock);

	WSACleanup();

	return 0;
}