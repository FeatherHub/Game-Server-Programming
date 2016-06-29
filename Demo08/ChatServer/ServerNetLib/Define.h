#pragma once

namespace NServerNetLib
{
	struct ServerConfig
	{
		unsigned short Port;
		int BackLogCount;

		int MaxClientCount;
		int ExtraClientCount; // 가능하면 로그인에서 짜르도록 MaxClientCount + 여유분을 준비한다.
		
		short MaxClientSockOptRecvBufferSize;
		short MaxClientSockOptSendBufferSize;
		short MaxClientRecvBufferSize;
		short MaxClientSendBufferSize;

		int MaxLobbyCount;
		int MaxLobbyUserCount;
		int MaxRoomCountByLobby;
		int MaxRoomUserCount;
	};

	const int MAX_IP_LEN = 32; // IP 문자열 최대 길이
	const int MAX_PACKET_SIZE = 1024; // 최대 패킷 크기
	//const int MAX_SOCK_OPT_RECV_BUUER_SIZE = MAX_PACKET_SIZE * 10; // 소켓 옵션용 받기 버퍼 크기
	//const int MAX_SOCK_OPT_SEND_BUUER_SIZE = MAX_PACKET_SIZE * 10; // 소켓 옵션용 보내기 버퍼 크기
	//const int MAX_CLIENT_RECV_BUFFER_SIZE = MAX_PACKET_SIZE * 8; // 클라이언트 받기용 최대 버퍼 크기
	//const int MAX_CLIENT_SEND_BUFFER_SIZE = MAX_PACKET_SIZE * 8; // 4k. 클라이언트 보내기용 최대 버퍼 크기

	struct ClientSession
	{
		bool IsConnected() { return SocketFD > 0 ? true : false; }

		void Clear()
		{
			Seq = 0;
			SocketFD = 0;
			IP[0] = '\0';
			RemainingDataSize = 0;
			SendSize = 0;
		}

		int Index = 0;
		long long Seq = 0;
		int		SocketFD = 0;
		char    IP[MAX_IP_LEN] = { 0, };

		char*   pRecvBuffer = nullptr;
		int     RemainingDataSize = 0;

		char*   pSendBuffer = nullptr;
		int     SendSize = 0;
	};

	struct RecvPacketInfo
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};

	struct RecvPacketInfo2
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};


	enum class SOCKET_CLOSE_CASE : short
	{
		SESSION_POOL_EMPTY = 1,
		SELECT_ERROR = 2,
		SOCKET_RECV_ERROR = 3,
		SOCKET_RECV_BUFFER_PROCESS_ERROR = 4,
		SOCKET_SEND_ERROR = 5,
	};
	

	enum class PACKET_ID : short
	{
		NTF_SYS_CLOSE_SESSION = 3,
				
	};

#pragma pack(push, 1)
	struct PacketHeader
	{
		short Id;
		short BodySize;
	};

	const int PACKET_HEADER_SIZE = sizeof(PacketHeader);


	struct PktNtfSysCloseSession : PacketHeader
	{
		int SockFD;
	};
#pragma pack(pop)
}