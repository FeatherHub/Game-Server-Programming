#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <Process.h>

#include <stdio.h>

const int READ = 3;
const int WRITE = 5;

const int BUFF_SIZE = 100;

const USHORT PORT_NUM = 23452;

struct SOCK_INFO
{
	SOCKET sock;
	SOCKADDR_IN addr;
};

struct SOCK_DATA
{
public:
	SOCK_DATA()
	{
		ZeroMemory(&overlapped, sizeof(overlapped));
		wsaBuf.len = BUFF_SIZE;
		wsaBuf.buf = buff;
		rwMode = READ;
	}

	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buff[BUFF_SIZE];
	int rwMode;
};

UINT __stdcall EchoThread(LPVOID pComPort);

int main()
{
	int res = 0;

	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (res != 0) 
		printf("WSAStartup err \n");

	
	HANDLE hComPort = CreateIoCompletionPort //1. IOCP 생성
	( 
		INVALID_HANDLE_VALUE, //IOCP와 연결할 핸들
							  //생성 시 INVALID_HANDLE_VALUE
		NULL, //IOCP 핸들
			  //생성 시 NULL

		0, //IO 완료시 넘어갈 값
		   //사용자가 정의한다

		0 //한 번에 동작할 수 있는 최대 스레드 개수
		  //0 전달하면 프로세서 숫자로 자동 지정된다.
	);
	
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	DWORD procNum = sysInfo.dwNumberOfProcessors;
	for(DWORD i = 0; 
		i < procNum * 2 + 1; //작업 처리 도중 blocking 함수를 만나게 되어
						     //스레드가 Paused Thread List에 들어가는 상황에도 
							 //바로 다른 작업을 처리할 수 있게 
							 //스레드 개수를 CPU 개수 보다 크게 잡는다.
		i++)
	{
		//IO작업이 완료된 후의 처리를 수행할 스레드 풀을 생성한다
		_beginthreadex(nullptr, 0,
						EchoThread, (LPVOID)hComPort,
						0, nullptr);
	}

	SOCKET hListenSock = WSASocket(
						AF_INET, SOCK_STREAM, 0,
						nullptr, 0,
						WSA_FLAG_OVERLAPPED);
	
	if (hListenSock == INVALID_SOCKET)
		printf("WSASocket err \n");

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(hListenSock, 
				(SOCKADDR*)&serverAddr,
				sizeof(serverAddr));

	if (res == SOCKET_ERROR) 
		printf("bind err \n");

	res = listen(hListenSock, SOMAXCONN);

	if (res == SOCKET_ERROR)
		printf("listen err \n");

	printf("Server run... \n");

	while (true)
	{
		SOCKET hClientSock;
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		hClientSock = accept(hListenSock,
							(SOCKADDR*)&clientAddr, 
							&addrLen);

		if (hClientSock == INVALID_SOCKET)
			printf("accept err \n");
		
		SOCK_INFO* pSockInfo = new SOCK_INFO();
		pSockInfo->sock = hClientSock;
		CopyMemory(&pSockInfo->addr, &clientAddr, addrLen);
		//sockInfo->addr = clientAddr;

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1); 
		printf("%s has connected \n", clientIP);

		//2. IO장치와 IOCP연결
		CreateIoCompletionPort(
			(HANDLE)hClientSock, //IOCP와 연결할 핸들

			hComPort, //IOCP 핸들

			(ULONG_PTR)pSockInfo, //사용자 정의 키

			0 //Released Thread List에 들어 갈 수 있는 최대 스레드 개수
			  //즉, 동시에 작업을 수행할 스레드의 개수
			  //스레드 당 CPU 하나가 가장 이상적인 퍼포먼스를 내기 때문에
			  //CPU의 개수에 맞춰 이 값을 세팅한다.
		);
	
		SOCK_DATA* pSockData = new SOCK_DATA();

		DWORD recvNum = 0;
		DWORD flag = 0;
		res = WSARecv(pSockInfo->sock,
				&pSockData->wsaBuf,
				1, //number of buff
				&recvNum,
				&flag,
				&pSockData->overlapped,
				nullptr);

		if (res == 0)
			printf("client left \n");
		else if (res == SOCKET_ERROR)
		{
			res = WSAGetLastError();
			if (res == WSA_IO_PENDING)
				printf("WSARecv WSA IO PENDING \n");
			else if (res == WSAEWOULDBLOCK)
				printf("WSARecv WSAEWOULDBLOCK \n");
			else
				printf("WSARecv err \n");
		}
	}

	return 0;
}

UINT __stdcall EchoThread(LPVOID pComPort)
{
	HANDLE hComPort = (HANDLE)pComPort;

	static int echoThreadRunNum = 0;
	printf("Thread %d run \n", echoThreadRunNum++);

	while (true)
	{
		DWORD recvNum = 0;
		SOCK_INFO* pSockInfo = nullptr;
		SOCK_DATA* pSockData = nullptr;

		int ret = GetQueuedCompletionStatus	 //스레드가 GQCS 호출하면
		(									 //Blocking되고
											 //Waiting Thread Queue에
											 //스레드 ID가 들어간다 
			hComPort, //handle of an iocp object								 
			&recvNum, //trans bytes					 
			(LPDWORD)&pSockInfo, //user defined key
			(LPOVERLAPPED*)&pSockData, //overlapped
			INFINITE //time out
		); 

//		if(ret == 0 && WSAGetLastError() == WAIT_TIMEOUT)
//			continue;

		SOCKET sock = pSockInfo->sock;

		if (pSockData->rwMode == READ)
		{
			if (recvNum == 0) 
			{
				char clientIP[32] = { 0, };
				inet_ntop(AF_INET, &(pSockInfo->addr.sin_addr), clientIP, 32 - 1);
				printf("%s has left \n", clientIP);
			
				closesocket(pSockInfo->sock);
				delete pSockInfo;
				delete pSockData;

				continue;
			}

			printf("Msg recved : %s \n", pSockData->buff);

			//echoing
			ZeroMemory(&pSockData->overlapped,
				sizeof(pSockData->overlapped));
			pSockData->wsaBuf.len = recvNum;
			pSockData->rwMode = WRITE;

			WSASend(sock,
				&pSockData->wsaBuf,
				1, //number of buff
				nullptr, //sent bytes buff
				0, //flag
				&pSockData->overlapped,
				nullptr);

			pSockData = new SOCK_DATA();
			DWORD flag = 0;
			WSARecv(sock,
				&pSockData->wsaBuf,
				1,
				nullptr,
				&flag,
				&pSockData->overlapped,
				nullptr);
		}
	}

	return 0;
}