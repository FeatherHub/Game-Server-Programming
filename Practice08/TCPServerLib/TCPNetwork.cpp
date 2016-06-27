#include "TCPNetwork.h"

namespace TCPServerLib
{
	TCPNetwork::TCPNetwork()
	{

	}

	TCPNetwork::~TCPNetwork()
	{

	}

	NET_ERROR_CODE TCPNetwork::Init(const ServerConfig* pConfig, ILog* pLogger)
	{

	}

	void TCPNetwork::Run()
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::SendData(const int sIdx, const short pktId, const short size, const char* pMsg)
	{

	}

	TCPServerLib::RecvPacketInfo TCPNetwork::GetPacketInfo()
	{

	}

	NET_ERROR_CODE TCPNetwork::InitServerPacket()
	{

	}

	NET_ERROR_CODE TCPNetwork::BindListen(short port, int backlogCount)
	{

	}

	void TCPNetwork::CreateSessionPool(const int maxClientCount)
	{

	}

	bool TCPNetwork::RunCheckSelectResult(const int result)
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::NewSession()
	{

	}

	int TCPNetwork::GetClientSessionIndex()
	{

	}

	void TCPNetwork::SetSockOption()
	{

	}

	void TCPNetwork::ConnectedSession()
	{

	}

	void TCPNetwork::RunCheckSelectClient(fd_set& exc_set, fd_set& read_set, fd_set& write_set)
	{

	}

	bool TCPNetwork::RunProcessReceive(const int sessionIdx, const SOCKET fd, fd_set& read_set)
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::ReceiveSocket(const int sessionIdx)
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::ReceiveBufferProcess(const int sessionIdx)
	{

	}

	bool TCPNetwork::RunProcessWrite(const int sessionIdx, const SOCKET fd, fd_set& write_set)
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::FlushSendBuffer(const int sessionIdx)
	{

	}

	TCPServerLib::NET_ERROR_CODE TCPNetwork::SendSocket(const SOCKET fd, const char* pMsg, const int size)
	{

	}

	void TCPNetwork::AddPacketQueue(const int sessionIdx, const short pktId, const short bodySize, char* pDataPos)
	{

	}

	void TCPNetwork::ReleaseSessionIndex(const int idx)
	{

	}

	void TCPNetwork::CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sock, const int sessionIdx)
	{

	}

}