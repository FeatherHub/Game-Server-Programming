// 이 소스는 '온라인 게임 네트워크 프로그래밍(대림출판서. 신동훈 저)에서 제공한 코드이다.

#ifndef  __Packet_H
#define  __Packet_H

#include <Windows.h>

#define		PACKETBUFFERSIZE	8192
#define		PACKETHEADERSIZE	4

class Packet
{
public:
			Packet();
			Packet( unsigned short idValue );
			Packet( const Packet& source );
	virtual ~Packet();

			bool			isValidHeader();
			bool			isValidPacket();

			Packet&			id( unsigned short ID );
			unsigned short	id();

			void			clear();

			unsigned short	getDataFieldSize();
			int				getPacketSize(){ return ( getDataFieldSize() + PACKETHEADERSIZE ); }
			int				getReceivedSize(){ return receivedSize; }
			char*			getPacketBuffer(){ return packetBuffer; }

			void			copyToBuffer( char* buff, int size );

			void			readData( void* buffer, int size );
			void			writeData( void* buffer, int size );


			//  Convenience packet I/O operators
			Packet&	operator = ( Packet& packet );

			//Write packet
			Packet&	operator << ( bool arg );
			Packet&	operator << ( int arg );
			Packet&	operator << ( long arg );
			Packet&	operator << ( DWORD arg );
			Packet& operator << ( __int64 arg );
			Packet&	operator << ( LPTSTR arg );
			Packet& operator << ( Packet& arg );

			//Read Packet
			Packet&	operator >> ( bool& arg );
			Packet&	operator >> ( int& arg );
			Packet&	operator >> ( long& arg );
			Packet&	operator >> ( DWORD& arg );
			Packet& operator >> ( __int64& arg );
			Packet& operator >> ( LPTSTR arg );
			Packet& operator >> ( Packet& arg );

protected:
	typedef struct
	{
		unsigned short*		dataSize;
		unsigned short*		protocolID;
	} HEADER;

			HEADER		packetHeader;

private:
			char		packetBuffer[PACKETBUFFERSIZE];
			char*		dataField;
			char*		readPosition, *writePosition;
			char*		endOfDataField;

			int			receivedSize;
};

#endif