#include "IOCPNetwork.h"

ERROR_CODE IOCPNetwork::Init(const char* IP, const short port)
{
	int res = 0;

	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
		return HandleError("WSAStartup()");

	m_listenSock = WSASocket(AF_INET, SOCK_STREAM, 0, 
					nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSock == INVALID_SOCKET) 
		return HandleError("WSASocket()");

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(m_listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (res == SOCKET_ERROR)
		return HandleError("bind()");

	m_ioComPort = CreateIOCP();

	InitClientStuff();

	m_initialized = true;

	return ERROR_CODE::NONE;
}

ERROR_CODE IOCPNetwork::Run()
{
	if (m_initialized == false)
		return ERROR_CODE::INIT_NOT_CALLED;

	if (m_isRunning == true)
		return ERROR_CODE::ALREADY_RUNIING;

	int res = listen(m_listenSock, SOMAXCONN);
	if (res == SOCKET_ERROR)
		return HandleError("listen()");

	//Run accept thread
	auto acceptThread = 
		std::thread(std::bind(&IOCPNetwork::AcceptThreadFunc, this));
	acceptThread.detach();

	//Run worker thread
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	auto threadNum = sysInfo.dwNumberOfProcessors * 2 + 1;
	for (int i = 0; i < threadNum; i++)
	{
		auto recvThread = 
			std::thread(std::bind(&IOCPNetwork::RecvThreadFunc, this));
		recvThread.detach();
	}

	//Run send thread
	auto sendThread =
		std::thread(std::bind(&IOCPNetwork::SendThreadFunc, this));
	sendThread.detach();

	m_isRunning = true;

	return ERROR_CODE::NONE;
}

void IOCPNetwork::AcceptThreadFunc()
{
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		//blocked!
		SOCKET clientSocket = accept(m_listenSock, (SOCKADDR*)&clientAddr, &addrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			HandleError("accept()");
			continue;
		}

		//클라이언트 수용량 초과
		while (m_clientIdxPool.empty() == true)
		{
			Sleep(1000);
		}

		m_idxPoolMutex.lock();
		auto newIdx = m_clientIdxPool.front();
		m_clientIdxPool.pop();
		m_idxPoolMutex.unlock();

		m_clientPoolMutex.lock();
		auto newClient = m_clientPool[newIdx];
		m_clientPoolMutex.unlock();

		InitClient(newClient, clientSocket, &clientAddr);

		BindSocketToCP(clientSocket);

		ClientData* pClientData = new ClientData();
		ZeroMemory(&pClientData->overlapped, sizeof(OVERLAPPED));
		pClientData->clientIdx = newIdx;
		pClientData->rwMode = RW_MODE::READ;
		pClientData->wsaBuf.buf = newClient.recvBuff;
		pClientData->wsaBuf.len = (ULONG)NetworkConfig::MAX_BUFF_SIZE;

		DWORD recvSize = 0;
		DWORD flag = 0;

		WSARecv(clientSocket,
			&pClientData->wsaBuf,
			1,
			&recvSize, &flag,
			&pClientData->overlapped,
			nullptr);
	}
}

void IOCPNetwork::RecvThreadFunc()
{
	ClientData* pClientData = nullptr;
	DWORD transBytes = 0;
	ULONG dummy = 0;
	while (true)
	{
		GetQueuedCompletionStatus(m_ioComPort, &transBytes, &dummy,
			(LPOVERLAPPED*)pClientData, INFINITE);

		auto clientIdx = pClientData->clientIdx;

		m_clientPoolMutex.lock();
		auto client = m_clientPool[clientIdx];
		m_clientPoolMutex.unlock();

		if (pClientData->rwMode == RW_MODE::READ)
		{
			if (transBytes == 0)
			{
				CloseClient(clientIdx);
				continue;
			}
			
			//여기서부터 select 서버와 병합하면서 작업 진행

		}
	}
}

void IOCPNetwork::SendThreadFunc()
{

}

HANDLE IOCPNetwork::CreateIOCP()
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
}

void IOCPNetwork::BindSocketToCP(SOCKET sock)
{
	CreateIoCompletionPort((HANDLE)sock, m_ioComPort, (ULONG_PTR)nullptr, 0);
}

void IOCPNetwork::InitClientStuff()
{
	for (int i = 0; i < (int)NetworkConfig::MAX_CLIENT_NUM; i++)
	{
		m_clientIdxPool.push(i);
	}
}

void IOCPNetwork::InitClient(Client& target, SOCKET sock, SOCKADDR_IN* addr)
{
	target.readPos = 0;
	target.recvSize = 0;
	target.sendSize = 0;
	target.sentSize = 0;
	target.socket = sock;

	if (addr != nullptr)
		inet_ntop(AF_INET, &(addr->sin_addr.s_addr), target.IP, INET_ADDRSTRLEN);
	else
		ZeroMemory(target.IP, 32);

	target.isConnected = true;
}

void IOCPNetwork::CloseClient(int clientIdx)
{
	m_clientPoolMutex.lock();
	auto& client = m_clientPool[clientIdx];
	closesocket(client.socket);
	InitClient(client, -1, nullptr);
	m_clientPoolMutex.unlock();

	m_idxPoolMutex.lock();
	m_clientIdxPool.push(clientIdx);
	m_idxPoolMutex.unlock();

	RecvPacket closeClientPkt;
	closeClientPkt.clientIdx = clientIdx;
	closeClientPkt.packetId = PacketId::NETWORK_CLOSE_CLIENT;
	m_recvPktQueue.Push(closeClientPkt);
}

ERROR_CODE IOCPNetwork::HandleError(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessageA
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	);

	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);

	return ERROR_CODE::SOCKET_FUNC_ERROR;
}