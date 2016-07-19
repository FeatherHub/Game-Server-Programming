#define FD_SETSIZE 1024

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdio.h>

const int BUFFER_SIZE = 512;
const int PORT = 23452;

struct Client
{
public:
	Client(SOCKET sock) : s(sock) 
	{}

	SOCKET s;
	char recvBuff[BUFFER_SIZE] = { 0, };
	char sendBuff[BUFFER_SIZE] = { 0, };
	int recvSize = 0;
	int sendSize = 0;
};

Client* clientArray[FD_SETSIZE];
int totalClientNum = 0;

SOCKET listenSock;

fd_set write_set;
fd_set read_set;
const timeval tv{ 0, 0 };

void InitNetwork();
int SelectSockets();
int ProcessSockets();
void CloseNetwork();

int AddSocket(SOCKET s);
int RemoveSocket(int idx);

int main()
{
	InitNetwork();

	while (true)
	{
		int ret = 0;

		ret = SelectSockets();
		//각 FD_Set의 소켓을 모두 검사하고 리턴한 경우
		if (ret == 0)
		{
			continue;
		}
		else if (ret == SOCKET_ERROR)
		{
			printf("Select Error! \n");
			break;
		}

		ret = ProcessSockets();
		if (ret != 0)
		{
			printf("Process Error! \n");
			break;
		}
	}

	CloseNetwork();

	return 0;
}

void InitNetwork()
{
	int ret = 0;

	WSADATA wd;
	ret = WSAStartup(MAKEWORD(2, 2), &wd);
	if (ret != 0)
	{
		printf("WSAStartUp Error! \n");
		exit(1);
	}

	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
	{
		printf("Listen socket Error! \n");
		exit(1);
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("Bind Error! \n");
		exit(1);
	}

	ret = listen(listenSock, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		printf("Listen Error! \n");
		exit(1);
	}

	ULONG on = 1;
	ret = ioctlsocket(listenSock, FIONBIO, &on);
	if (ret == SOCKET_ERROR)
	{
		printf("Nonblocking Error! \n");
		exit(1);
	}
}

int SelectSockets()
{
	FD_ZERO(&write_set);
	FD_ZERO(&read_set);

	FD_SET(listenSock, &read_set);

	//Client들을 각 FD_Set에 넣는다
	for (int i = 0; i < totalClientNum; i++)
	{
		//보내야할 데이터가 있으면 쓰기셋에 넣는다
		if (clientArray[i]->sendSize > 0)
		{
//			printf("client %d is put to write set \n");
			FD_SET(clientArray[i]->s, &write_set);
		}

		//버퍼에 여유가 있어 읽을 수 있으면 읽기셋에 넣는다
		if (clientArray[i]->recvSize != BUFFER_SIZE)
		{
//			printf("client %d is put to read set \n");
			FD_SET(clientArray[i]->s, &read_set);
		}
	}

	return select(0, &read_set, &write_set, nullptr, &tv);
}

int ProcessSockets()
{
	int ret = 0;

	//클라이언트 수용
	SOCKET clientSock = accept(listenSock, nullptr, nullptr);
	if (clientSock == INVALID_SOCKET)
	{
		//The socket is marked as nonblocking 
		//and no connections are present to be accepted.
		if (WSAEWOULDBLOCK != WSAGetLastError())
		{
			return 1;
		}
	}
	else
	{ 
		printf("Welcome! \n");
		ret = AddSocket(clientSock);
		if (ret != 0)
		{
			return 1;
		}
	}

	/*클라이언트들에 대해서 처리*/
	//읽기
	for (int i = 0; i < totalClientNum; i++)
	{
		Client* client = clientArray[i];
		SOCKET s = client->s;

		if (FD_ISSET(s, &read_set))
		{
			printf("client %d is reading ... \n");

			int ret = 0;
			ret = recv(s, client->recvBuff + client->recvSize, 
						BUFFER_SIZE - client->recvSize, 0);
			
			if (ret == 0)
			{
				printf("ByeBye! \n");
				RemoveSocket(i);
				continue;
			}
			else if (ret == SOCKET_ERROR)
			{
				printf("Recv Error! \n");
				return 1;
			}

			printf("client %d send me %s! \n", i, client->recvBuff);

			//recv buffer process
			client->recvSize += ret;

			//overflow가 아닌 경우에 send buffer에 복사한다
			if(client->recvSize < (BUFFER_SIZE - client->sendSize))
			{
				memcpy(client->sendBuff + client->sendSize, 
					client->recvBuff, client->recvSize);

				client->sendSize += client->recvSize;

				ZeroMemory(client->recvBuff, client->recvSize);
				
				//순서주의: 더해주고 나서 0으로 바꾼다
				client->recvSize = 0;
			}
		}
	}

	//쓰기
	for (int i = 0; i < totalClientNum; i++)
	{
		Client* client = clientArray[i];
		SOCKET s = client->s;

		if (FD_ISSET(s, &write_set))
		{
			printf("client %d is writing ... \n");

			int ret = 0;

			ret = send(s, client->sendBuff, client->sendSize, 0);
			if (ret == SOCKET_ERROR)
			{
				printf("Send Error! \n");
				return 1;
			}

			//send buffer process
			client->sendSize -= ret;

			memcpy(client->sendBuff,
				client->sendBuff + ret, client->sendSize);
		}
	}

	return 0;
}

void CloseNetwork()
{
	for (int i = 0; i < totalClientNum; i++)
	{
		closesocket(clientArray[i]->s);
	}

	closesocket(listenSock);

	WSACleanup();
}

int AddSocket(SOCKET s)
{
	Client* client = new Client(s);
	if (client == nullptr)
	{
		return 1;
	}

	clientArray[totalClientNum++] = client;

	return 0;
}

int RemoveSocket(int idx)
{
	//out of index
	if (idx < 0 || idx >= totalClientNum)
	{
		return 1;
	}

	closesocket(clientArray[idx]->s);

	//마지막에 있던 원소가 아니면 빈 자리를 채운다
	if (idx != totalClientNum - 1)
	{
		clientArray[idx] = clientArray[totalClientNum - 1];
	}

	totalClientNum--;

	return 0;
}

