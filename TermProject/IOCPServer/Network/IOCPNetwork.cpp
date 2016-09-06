#include "IOCPNetwork.h"
#include "NetPacket.h"

#include "..\..\Common\Util\BodySizeMananger.h"
#include "..\..\Common\Util\Logger.h"
#include "..\..\Common\Packetid.h"

NETCODE IOCPNetwork::Init(const short port)
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

	m_bodySizeMgr = new BodySizeManager();
	
	m_initialized = true;

	Logger::Write(Logger::INFO, "IOCP Network init success");

	return NETCODE::NONE;
}

NETCODE IOCPNetwork::Run()
{
	if (m_initialized == false)
		return NETCODE::INIT_NOT_CALLED;

	if (m_isRunning == true)
		return NETCODE::ALREADY_RUNIING;

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
	for (UINT i = 0; i < threadNum; i++)
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

	Logger::Write(Logger::INFO, "IOCP Network run..");

	return NETCODE::NONE;
}

void IOCPNetwork::SendPacket(int clientIdx, unsigned short pktid, char* dataPos)
{
	using namespace NetworkLib;

	m_bodySizeMgrMutex.lock();
	int bodySize = m_bodySizeMgr->Get(pktid);
	m_bodySizeMgrMutex.unlock();

	NetPacket recvPkt;
	recvPkt.clientIdx = clientIdx;
	recvPkt.id = pktid;
	recvPkt.pDataAtBuff = dataPos;

	m_sendPktQueue.Push(recvPkt, bodySize);
}

IOCPNetwork::NetPacket IOCPNetwork::GetPacket()
{
	return m_recvPktQueue.GetPacket();
}

bool IOCPNetwork::PacketQueueEmpty()
{
	return m_recvPktQueue.Empty();
}

std::queue<int>& IOCPNetwork::GetClosedClientQueue()
{
	return m_closedIdxPool;
}

std::mutex& IOCPNetwork::GetClosedClientQueueMutex()
{
	return m_closedPoolMutex;
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
		Client& newClient = m_clientPool[newIdx];
		m_clientPoolMutex.unlock();

		InitClient(newClient, clientSocket, &clientAddr, true);

		BindSocketToCP(clientSocket);

		m_clientNum++;

		Logger::Write(Logger::INFO, "%dth %s connected", m_clientNum, newClient.IP);

		ClientData* pClientData = new ClientData();
		ZeroMemory(&pClientData->overlapped, sizeof(OVERLAPPED));
		pClientData->clientIdx = newIdx;
		pClientData->rwMode = RW_MODE::READ;
		pClientData->wsaBuf.buf = newClient.recvBuff;
		pClientData->wsaBuf.len = (ULONG)NetConfig::MAX_BUFF_SIZE;

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
	using namespace NetworkLib;

	ClientData* pClientData = nullptr;
	DWORD transBytes = 0;
	ULONG_PTR dummy = 0U;

	while (true)
	{
		GetQueuedCompletionStatus(m_ioComPort, &transBytes, &dummy,
			(LPOVERLAPPED*)&pClientData, INFINITE);

		auto clientIdx = pClientData->clientIdx;

		m_clientPoolMutex.lock();
		Client& c = m_clientPool[clientIdx]; //참조 괜찮나?
		m_clientPoolMutex.unlock();
		
		if (pClientData->rwMode == RW_MODE::READ)
		{
			if (transBytes == 0)
			{
				CloseClient(clientIdx);
				continue;
			}
			
			//전처리
			c.recvSize += transBytes;

			while (true)
			{
				if (c.recvSize < PACKET_HEADER_SIZE)
				{
					break;
				}

				auto pktHeader = (PktHeader*)(c.recvBuff + c.readPos);
				
				m_bodySizeMgrMutex.lock();
				int bodySize = m_bodySizeMgr->Get(pktHeader->id);
				m_bodySizeMgrMutex.unlock();

				if (c.recvSize < PACKET_HEADER_SIZE + bodySize)
				{
					break;
				}

				NetPacket recvPkt;
				recvPkt.clientIdx = clientIdx;
				recvPkt.id = pktHeader->id;
				recvPkt.pDataAtBuff = c.recvBuff + c.readPos + PACKET_HEADER_SIZE;
				
				m_recvPktQueue.Push(recvPkt, bodySize); //thread safe & copy data

				//후처리1
				c.recvSize -= PACKET_HEADER_SIZE + bodySize;
				c.readPos += PACKET_HEADER_SIZE + bodySize;
			}

			//후처리2 - 패킷데이터를 Queue에 넣을 때 복사하기 때문에 바로 당긴다.
			CopyMemory(c.recvBuff, c.recvBuff + c.readPos, c.recvSize);
			c.readPos = 0;

			//Recv 요청
			ZeroMemory(&pClientData->overlapped, sizeof(OVERLAPPED));
			pClientData->wsaBuf.buf = c.recvBuff + c.recvSize;
			pClientData->wsaBuf.len = (ULONG)NetConfig::MAX_BUFF_SIZE - c.recvSize;
			pClientData->rwMode = RW_MODE::READ;

			DWORD recvSize = 0;
			DWORD flag = 0;

			WSARecv(c.socket,
				&pClientData->wsaBuf,
				1,
				&recvSize, &flag, 
				&pClientData->overlapped, 
				nullptr);
		}
	}
}

void IOCPNetwork::SendThreadFunc()
{
	using namespace std;
	using namespace NetworkLib;

	while (true)
	{
		if (m_sendPktQueue.Empty() == true)
		{
			this_thread::sleep_for(chrono::milliseconds(0)); //<thread> has <chrono>
			continue;
		}
		
		auto recvPkt = m_sendPktQueue.GetPacket(); //thread safe
		if(recvPkt.id == PacketId::EMPTY_PKT)
			continue;

		m_clientPoolMutex.lock();
		Client& c = m_clientPool[recvPkt.clientIdx]; //참조인데 괜찮을까? - 데이터 공유 문제
		m_clientPoolMutex.unlock();
		
		m_bodySizeMgrMutex.lock();
		int bodySize = m_bodySizeMgr->Get(recvPkt.id);
		m_bodySizeMgrMutex.unlock();

		send(c.socket, (char*)&recvPkt.id, PACKET_HEADER_SIZE, 0);
		send(c.socket, recvPkt.pDataAtBuff, bodySize, 0);
	}
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
	for (int i = 0; i < (int)NetConfig::MAX_CLIENT_NUM; i++)
	{
		m_clientIdxPool.push(i);
	}

	m_clientNum = 0;
}

void IOCPNetwork::InitClient(Client& target, SOCKET sock, SOCKADDR_IN* addr, bool isConnected)
{
	target.readPos = 0;
	target.recvSize = 0;
	target.sendSize = 0;
	target.sentSize = 0;
	target.socket = sock;

	if (addr == nullptr)
		ZeroMemory(target.IP, 32);
	else
		inet_ntop(AF_INET, &(addr->sin_addr.s_addr), target.IP, INET_ADDRSTRLEN);

	target.isConnected = isConnected;
}

void IOCPNetwork::CloseClient(int clientIdx)
{
	//데이터 삭제
	m_clientPoolMutex.lock();
	auto& client = m_clientPool[clientIdx];
	closesocket(client.socket);
	InitClient(client, 0, nullptr, false);
	m_clientPoolMutex.unlock();

	//인덱스 반환
	m_idxPoolMutex.lock();
	m_clientIdxPool.push(clientIdx);
	m_idxPoolMutex.unlock();

//	//종료알림 패킷
//	m_bodySizeMgrMutex.lock();
//	int bodySize = m_bodySizeMgr->Get(PacketId::RemoveUserNtf);
//	m_bodySizeMgrMutex.unlock();
//
//	NetworkLib::NetPacket closeClientPkt;
//	closeClientPkt.clientIdx = clientIdx;
//	closeClientPkt.id = PacketId::RemoveUserNtf;
//	closeClientPkt.pDataAtBuff = new char[bodySize];
//	m_recvPktQueue.Push(closeClientPkt, bodySize); //thread safe

	Logger::Write(Logger::INFO, "cIdx %d is sent to tomb", clientIdx);

	//종료큐에 추가
	m_closedPoolMutex.lock();
	m_closedIdxPool.push(clientIdx);
	m_closedPoolMutex.unlock();
}

NETCODE IOCPNetwork::HandleError(const char* msg)
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

	return NETCODE::SOCKET_FUNC_ERROR;
}