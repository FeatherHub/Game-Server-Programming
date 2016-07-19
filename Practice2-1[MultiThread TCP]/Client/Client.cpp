//library
#pragma comment(lib, "ws2_32")

//winsock header
#include <WinSock2.h>
#include <Ws2tcpip.h>

//etc header
#include <stdlib.h>
#include <stdio.h>

//constants
const char* SERVER_IP = "127.0.0.1";
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

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received = 0;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main()
{
	int retVal = 0;

	//winsock 
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	//sock
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		DisplayError("Sock error");
		return 1;
	}

	//connect
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_IP, (void*)&serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(SERVER_PORT);

	retVal = connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		DisplayError("Connect error");
		return 1;
	}

	char buf[BUFFER_SIZE + 1] = { 0, };
	int len = 0;
	
	while (true)
	{
		//input
		if (fgets(buf, BUFFER_SIZE + 1, stdin) == NULL)
		{
			break;
		}

		len = strlen(buf);
		if (buf[len - 1] == '\n')
		{
			buf[len - 1] = '\0';
		}

		//send
		retVal = send(sock, buf, strlen(buf), 0);
		if (retVal == SOCKET_ERROR)
		{
			DisplayError("Send error");
			break;
		}

		//receive (echo)
		retVal = recvn(sock, buf, retVal, 0);
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
		printf("%s (%d bytes) \n", buf, retVal);
	}

	//close socket
	closesocket(sock);

	//close winsock
	WSACleanup();

	return 0;
}