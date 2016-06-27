//lib1
#pragma comment(lib, "ws2_32")

//lib2
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

//constants
const int BUFFER_SIZE = 512;
const int SERVER_PORT = 23452;

//SockInfo structure
struct SockInfo
{
	SOCKET sock;
	char buf[BUFFER_SIZE];
	int recvBytes;
	int sendBytes;
};

SockInfo* sockInfoArr[FD_SETSIZE];
int sockInfoNum = 0;

//datas
bool AddSocketInfo(SOCKET sock);
void RemoveSockInfoAt(int i);

//main
int main()
{
	int ret;

	//wsa
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return 1;
	}
	
	//socket
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
	{
		return 1;
	}

	//SOCKADDR
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind
	ret = bind(listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		return 1;
	}

	//listen
	ret = listen(listenSock, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		return 1;
	}

	//non blocking
	u_long on = 1;
	ret = ioctlsocket(listenSock, FIONBIO, &on);
	if (ret == SOCKET_ERROR)
	{
		return 1;
	}

	//datas
	FD_SET readSet, writeSet;
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	int i;

	//main loop
	while (true)
	{
		//init fds
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		//add sock to fds
		FD_SET(listenSock, &readSet);

		for (i = 0; i < sockInfoNum; i++)
		{
			SockInfo* sockInfo = sockInfoArr[i];
			if (sockInfo->recvBytes > sockInfo->sendBytes)
			{
				FD_SET(sockInfo->sock, &readSet);
			}
			else
			{
				FD_SET(sockInfo->sock, &writeSet);
			}
		}

		//select
		ret = select(0, &readSet, &writeSet, NULL, NULL);
		if (ret == SOCKET_ERROR)
		{
			break;
		}

		//fd loop
		if (FD_ISSET(listenSock, &readSet))
		{
			clientSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSock == INVALID_SOCKET)
			{
				break;
			}

			AddSocketInfo(clientSock);
		}

		for (i = 0; i < sockInfoNum; i++)
		{
			SockInfo* sockInfo = sockInfoArr[i];

			//is read
			if (FD_ISSET(sockInfo->sock, &readSet))
			{
				//recv
				ret = recv(sockInfo->sock, sockInfo->buf, BUFFER_SIZE, 0);
				if (ret == SOCKET_ERROR)
				{
					RemoveSockInfoAt(i);
					continue;
				}

				sockInfo->sendBytes = ret;
				sockInfo->buf[ret] = '\0';

				printf("%s \n", sockInfo->buf);
			}

			//is write
			if(FD_ISSET(sockInfo->sock, &writeSet))
			{
				//send
				ret = send(sockInfo->sock, sockInfo->buf, sockInfo->sendBytes, 0);
				if (ret == SOCKET_ERROR)
				{
					RemoveSockInfoAt(i);
					continue;
				}
			}
		} //end fd loop
	} //end main loop

	//release datas
	closesocket(listenSock);

	WSACleanup();

	return 0;
} //main end

bool AddSocketInfo(SOCKET sock)
{
	//overflow
	if (sockInfoNum >= FD_SETSIZE)
	{
		return false;
	}

	SockInfo* sockInfo = new SockInfo;
	//allocate fail
	if (sockInfo == nullptr)
	{
		return false;
	}

	//init
	sockInfo->sock = sock;
	sockInfo->sendBytes = 0;
	sockInfo->recvBytes = 0;
	
	sockInfoArr[sockInfoNum++] = sockInfo;

	return true;
}

void RemoveSockInfoAt(int i)
{
	//index valid check

	SockInfo* sockInfo = sockInfoArr[i];

	//element valid check

	closesocket(sockInfo->sock);
	delete sockInfo;

	if (i != (sockInfoNum - 1))
	{
		sockInfoArr[i] = sockInfoArr[sockInfoNum - 1];
	}

	sockInfoNum--;
}