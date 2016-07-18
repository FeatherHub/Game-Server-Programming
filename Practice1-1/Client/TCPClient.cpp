//클라이언트

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <iostream>

using namespace std;

const string SERVER_IP = "127.0.0.1";
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

//데이터 수신 함수
int ReceiveNumber(SOCKET s, string buf, int len, int flags)
{
	int received = 0;
	int left = len;
	char* ptr = (char*)buf.c_str();

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}

		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main()
{
	int returnValue;

	cout << "[TCP Client] " << endl;

	//윈속 세팅
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	//소켓 세팅
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		DisplayError("create socket error");
	}

	//연결 세팅
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;

	inet_pton(AF_INET, SERVER_IP.c_str(), (void*)&serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(SERVER_PORT);

	returnValue = connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	if (returnValue == SOCKET_ERROR)
	{
		DisplayError("connect error");
		return -1;
	}

	//통신 준비
	char buf[BUFFER_SIZE];
	int len = 0;

	while (true)
	{
		//데이터 입력
		cout << "Message: ";
		cin.getline(buf, BUFFER_SIZE);

		len = strlen(buf);
		if (buf[len - 1] == '\n')
		{
			buf[len - 1] = '\0';
		}

		//데이터 전송
		returnValue = send(sock, buf, len, 0);
		
		//데이터 예외
		if (returnValue == SOCKET_ERROR)
		{
			DisplayError("send error");
			break;
		}

		cout << "[TCP Client] : Send " << returnValue << " byte" << endl;

		//데이터 수신
		//returnValue = ReceiveNumber(sock, buf, returnValue, 0);
		returnValue = recv(sock, buf, sizeof(buf), 0);


		//데이터 예외
		if (returnValue == SOCKET_ERROR)
		{
			DisplayError("receive error");
			break;
		}
		else if (returnValue == 0)
		{
			//break;
		}

		//데이터 출력
		cout << "[TCP Client] : Receive " << returnValue << " byte" << endl;
		cout << "[Receive Data] : " << buf << endl;
	}

	//소켓 정리
	closesocket(sock);

	//윈속 종료
	WSACleanup();

	return 0;
}