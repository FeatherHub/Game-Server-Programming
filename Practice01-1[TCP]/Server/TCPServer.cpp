//에코 서버

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <string>
#include <iostream>

using namespace std;

const int SERVER_PORT = 23452;
const int BUFFER_SIZE = 512;

//오류 출력
void DisplayError(char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	MessageBox(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

int main()
{
	int returnValue;

	//윈속 준비
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	//소켓 준비
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		DisplayError("create socket error");
		return -1;
	}

	//바인드 준비
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);

	returnValue = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (returnValue == SOCKET_ERROR)
	{
		DisplayError("bind error");
		return -1;
	}

	//리슨 준비
	returnValue = listen(listen_sock, SOMAXCONN);
	if (returnValue == SOCKET_ERROR)
	{
		DisplayError("listen error");
		return -1;
	}

	//통신 준비
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	char buf[BUFFER_SIZE] = { 0, };
	int clientAddrSize = 0;

	while (true)
	{
		//어셉트 준비
		clientAddrSize = sizeof(clientAddr);
		client_sock = accept(listen_sock, 
			(SOCKADDR*)&clientAddr, 
			&clientAddrSize);

		if (client_sock == INVALID_SOCKET)
		{
			DisplayError("accept error");
			break;
		}

		//연결된 클라이언트 정보 출력
		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
		cout << "[TCP Server] Connected client IP : " << clientIP << endl;
		
		class Packet
		{
			int packetId;
			int bodySize;
		};

		//데아터 통신
		while (true)
		{
			//데이터 수신
			returnValue = recv(client_sock, buf, BUFFER_SIZE, 0);
			if (returnValue == SOCKET_ERROR)
			{
				DisplayError("receive error");
				break;
			}

			buf[returnValue] = '\0';
			cout << "Received data : " << buf << endl;

			for (int i = 0; i < returnValue; i++)
			{
				cout << buf[i] << endl;
			}

			//데이터 전송
			returnValue = send(client_sock, buf, returnValue, 0);
			if (returnValue == SOCKET_ERROR)
			{
				DisplayError("send error");
				break;
			}

		} //통신 종료
		
		//클라 소켓 제거
		closesocket(client_sock);
		cout << "Connection closed : " << clientIP << endl;
	}

	closesocket(listen_sock);

	WSACleanup();

	return 0;
}