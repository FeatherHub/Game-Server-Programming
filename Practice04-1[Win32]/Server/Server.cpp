//Library
#pragma comment(lib, "ws2_32")

//Header
#include <Winsock2.h>
#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

//Constants
const int SERVER_PORT = 23452;
const int BUFFER_SIZE = 512;

//Signatures
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DisplayText(char *fmt, ...);

void DisplayError(char* msg);

DWORD WINAPI ServerMain(LPVOID msg);

DWORD WINAPI ProcessClient(LPVOID arg);

//Globals
HINSTANCE g_hInst;
HWND g_hEdit;
CRITICAL_SECTION g_cs;

//WinMain
int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPreInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	g_hInst = hInstance;
	InitializeCriticalSection(&g_cs);
	
	//Init WndClass
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "MyWndClass";

	if (!RegisterClass(&wndClass))
	{
		return 1;
	}

	//Create Window
	HWND hWnd = CreateWindow(
		"MyWndClass",
		"TCP Server",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		600,
		200,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hWnd == NULL)
		return 1;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//CreateThread : ServerMain
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	//Message Loop
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteCriticalSection(&g_cs);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		g_hEdit = CreateWindow(
			"edit",
			NULL,
			WS_CHILD | WS_VISIBLE | 
			WS_HSCROLL | WS_VSCROLL | 
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
			ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, g_hInst, NULL);
		return 0;
	
	case WM_SIZE:
		MoveWindow(g_hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_SETFOCUS:
		SetFocus(g_hEdit);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFFER_SIZE + 256];
	vsprintf_s(cbuf, sizeof(cbuf) - 1, fmt, arg);

	EnterCriticalSection(&g_cs);
	
	int len = GetWindowTextLength(g_hEdit);
	SendMessage(g_hEdit, EM_SETSEL, len, len);
	SendMessage(g_hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	
	LeaveCriticalSection(&g_cs);

	va_end(arg);
}

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
DWORD WINAPI ServerMain(LPVOID msg)
{
	int retVal;

	//Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}

	//Socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		DisplayError("socket()");
		return 1;
	}

	//SOCKADDR_IN member
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind socket and SOCKADDR_IN
	retVal = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		DisplayError("bind()");
		return 1;
	}

	//Listen
	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR)
	{
		DisplayError("listen()");
		return 1;
	}

	//Network datas
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	HANDLE hThread;

	while (true)
	{
		//Accept
		client_sock = accept(listen_sock, (SOCKADDR*)&clientAddr, &addrLen);
		if (client_sock == INVALID_SOCKET)
		{
			DisplayError("accept()");
			break;
		}

		//Client information
		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
		DisplayText("\r\n [TCP SERVER] Client Connected : IP=%s, Port=%d \r\n",
			clientIP, ntohs(clientAddr.sin_port));

		//CreateThread for network
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
	}

	//Free memories
	closesocket(listen_sock);

	WSACleanup();
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFFER_SIZE + 1];
	int retVal;

	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	while (true)
	{
		retVal = recv(client_sock, buf, BUFFER_SIZE, 0);
		if (retVal == SOCKET_ERROR)
		{
			DisplayError("recv()");
			break;
		}
		else if (retVal == 0)
		{
			break;
		}

		buf[retVal] = '\0';

		DisplayText("%s \r\n", buf);

		retVal = send(client_sock, buf, retVal, 0);
		if (retVal == SOCKET_ERROR)
		{
			DisplayError("send()");
			break;
		}
	}

	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, 32 - 1);
	DisplayText("[TCP SERVER] Client Disconnected IP=%s Port=%d \r\n", clientIP, ntohs(clientAddr.sin_port));

	closesocket(client_sock);

	return 0;
}