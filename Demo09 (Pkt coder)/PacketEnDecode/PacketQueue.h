#pragma once

//각종 데이터타입에 대한 별칭을 사용하기 위해 포함한다
#include <Windows.h>

#define		PACKETBUFFERSIZE	8192
#define		PACKETHEADERSIZE	4

//현재 파악한 Packet 클래스의 용도
//1. 임의의 크기를 갖는(최대 8192) 패킷을 하나 저장한다
//2. 패킷은 PacketHeader + DataField로 구성된다.
//3. 임의의 데이터형을 바이트화하여 버퍼에 저장한다
//	void*(모든 형을 수용가능)를 사용해서 가능하다.
//	Offset이 1인 순수한 위치다
class PacketQueue
{
public:
	PacketQueue();
	PacketQueue(unsigned short idValue); //Packet ID로 초기화
	PacketQueue(const PacketQueue& source); //복사 생성자
	virtual ~PacketQueue(); 

	//각각 크기 초과/미달을 검사
	bool			isValidHeader();
	bool			isValidPacket();

	//패킷아이디를 설정
	PacketQueue&			id(unsigned short ID);
	//패킷아디이를 get
	unsigned short	id();

	//패킷데이터를 리셋
	void			clear();

	//PacketHeader->dataSize
	unsigned short	getDataFieldSize();
	//dataSize + headerSize
	int				getPacketSize() { return (getDataFieldSize() + PACKETHEADERSIZE); }
	
	int				getReceivedSize() { return receivedSize; }
	char*			getPacketBuffer() { return packetBuffer; }

	//[buffer, buffer+size] -> member buffer
	void			copyToBuffer(char* buff, int size);

	//[buffer, buffer+size] <-> member buffer
	void			readData(void* buffer, int size);
	void			writeData(void* buffer, int size);

	// Packet의 내용을 깊은 복사한다
	PacketQueue&	operator = (PacketQueue& packet);

	//symmetric pairs of read and write
	//bool
	//short, int, long, DWORD, __int64
	//char*
	//float, double
	//a struct, Packet

	//Write Data to Packet
	PacketQueue&	operator << (bool arg);
	PacketQueue&	operator << (int arg);
	PacketQueue&	operator << (long arg);
	PacketQueue&	operator << (DWORD arg);
	PacketQueue& operator << (__int64 arg);
	PacketQueue&	operator << (LPTSTR arg);
	PacketQueue& operator << (PacketQueue& arg);

	//Read data From Packet
	PacketQueue&	operator >> (bool& arg);
	PacketQueue&	operator >> (int& arg);
	PacketQueue&	operator >> (long& arg);
	PacketQueue&	operator >> (DWORD& arg);
	PacketQueue& operator >> (__int64& arg);
	PacketQueue& operator >> (LPTSTR arg);
	PacketQueue& operator >> (PacketQueue& arg);

protected:
	//Packet ID & Data size
	typedef struct
	{
		unsigned short*		dataSize;
		unsigned short*		protocolID;
	} HEADER;

	HEADER		packetHeader;

private:
	//Receive & Write를 위한 (임시) 작업 공간
	char		packetBuffer[PACKETBUFFERSIZE];

	//data 위치
	char*		dataField;

	//readPos, writePos
	char*		readPosition, *writePosition;
	
	//dataField + dataSize
	char*		endOfDataField;

	int			receivedSize;
};
