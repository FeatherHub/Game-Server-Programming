//Library
#pragma comment(lib, "ws2_32")
//Network headers
#include <WinSock2.h>
#include <WS2tcpip.h>
//ETC headers
#include <stdlib.h>
#include <stdio.h>

const int SERVER_PORT = 23452;
const int BUFFER_SIZE = 512;

void DisplayError(char* msg)
{
	LPVOID lpMsgBuf;
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL);

	MessageBox(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg;
	int retVal = 0;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	char buf[BUFFER_SIZE + 1] = { 0, };
	char clientIP[32] = { 0, };

	getpeername(clientSock, (SOCKADDR*)&clientAddr, &addrLen);
	inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);

	while (true)
	{
		retVal = recv(clientSock, buf, BUFFER_SIZE, 0);
		if (retVal == SOCKET_ERROR)
		{
			DisplayError("Receive error");
			break;
		}
		else if (retVal == 0)
		{
			break;
		}

		buf[retVal] = '\0';
		printf("[TCP %s:%d] : %s \n", clientIP, ntohs(clientAddr.sin_port), buf);

		retVal = send(clientSock, buf, retVal, 0);
		if (retVal == SOCKET_ERROR)
		{
			DisplayError("Send error");
			break;
		}
	}

	closesocket(clientSock);

	return 0;
}

int main()
{
	int retVal = 0;

	//Init winsock 
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 1;

	//Init socket
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
	{
		DisplayError("Init socket error");
		return 1;
	}

	//Bind
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);
	
	retVal = bind(listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		DisplayError("Bind error");
		return 1;
	}

	//Listen
	retVal = listen(listenSock, SOMAXCONN);
	if (retVal == SOCKET_ERROR)
	{
		DisplayError("Listen error");
		return 1;
	}

	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	HANDLE hThread;
	
	while (true)
	{
		//Accept
		clientSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET)
		{
			DisplayError("Accept error");
			break;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
		printf("[TCP SERVER] client accepted : IP=%s PORT=%d \n", clientIP, ntohs(clientAddr.sin_port));

		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)clientSock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(clientSock);
		}
		else
		{
			CloseHandle(hThread);
		}
	}
	
	closesocket(listenSock);

	WSACleanup();

	return 0;
}