#pragma once

#pragma comment(lib, "ws2_32")

#define FD_SETSIZE 1024
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <deque>

#include "ITcpNetwork.h"

namespace TCPServerLib
{
	class TCPNetwork : ITcpNetwork
	{
	public:
		TCPNetwork();
		~TCPNetwork();

		NET_ERROR_CODE Init(const ServerConfig* pConfig, ILog* pLogger) override;
		void Run() override;
		NET_ERROR_CODE SendData(const int sIdx, const short pktId, const short size, const char* pMsg) override;
		RecvPacketInfo GetPacketInfo() override;
	
	protected:
		NET_ERROR_CODE InitServerPacket();
		NET_ERROR_CODE BindListen(short port, int backlogCount);
		void CreateSessionPool(const int maxClientCount);

		bool RunCheckSelectResult(const int result);
		
		NET_ERROR_CODE NewSession();
		int GetClientSessionIndex();
		void SetSockOption();
		void ConnectedSession();
		
		void RunCheckSelectClient(fd_set& exc_set, fd_set& read_set, fd_set& write_set);

		bool RunProcessReceive(const int sessionIdx, const SOCKET fd, fd_set& read_set);
		NET_ERROR_CODE ReceiveSocket(const int sessionIdx);
		NET_ERROR_CODE ReceiveBufferProcess(const int sessionIdx);

		bool RunProcessWrite(const int sessionIdx, const SOCKET fd, fd_set& write_set);
		NET_ERROR_CODE FlushSendBuffer(const int sessionIdx);
		NET_ERROR_CODE SendSocket(const SOCKET fd, const char* pMsg, const int size);

		void AddPacketQueue(const int sessionIdx, const short pktId, const short bodySize, char* pDataPos);

		void ReleaseSessionIndex(const int idx);
		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sock, const int sessionIdx);

	protected:
		ServerConfig m_config;

		SOCKET m_serverSocket;
		
		fd_set m_readSet;
		
		std::vector<ClientSession> m_clientSessionPool;
		std::vector<int> m_clientSessionPoolIndex;
		size_t m_connectedSessionCount = 0;
		int64_t m_connectSeq = 0;

		std::deque<RecvPacketInfo> m_packetQueue;

		ILog* m_pLogger;
	};
}