#include "PacketQueue.h"

PacketQueue::PacketQueue()
	: dataField( 0 ), readPosition( 0 ), writePosition( 0 ), receivedSize( 0 )
{
	clear();
}

PacketQueue::PacketQueue( unsigned short idValue )
	: dataField( 0 ), readPosition( 0 ), writePosition( 0 ), receivedSize( 0 )
{
	clear();
	id( idValue );
}

PacketQueue::PacketQueue( const PacketQueue& source )
	: dataField( 0 ), readPosition( 0 ), writePosition( 0 ),
	 receivedSize( 0 )
{
	clear();

	//버퍼 데이터 복사
	::CopyMemory( packetBuffer, source.packetBuffer, PACKETBUFFERSIZE );

	//멤버 변수 복사
	receivedSize = source.receivedSize;

	//버퍼 프로세스 변수 복사
	DWORD offset;

	offset = ( DWORD )source.readPosition - ( DWORD )source.dataField;
	readPosition += offset;

	offset = ( DWORD )source.writePosition - ( DWORD )source.dataField;
	writePosition += offset;
}

PacketQueue::~PacketQueue()
{
}

bool PacketQueue::isValidHeader()
{
	return ( getPacketSize() >= PACKETHEADERSIZE );
}

bool PacketQueue::isValidPacket()
{
	if( isValidHeader() == false )
		return false;

	return ( getPacketSize() <= receivedSize );
}

void PacketQueue::clear()
{
	::ZeroMemory( packetBuffer, PACKETBUFFERSIZE );

	packetHeader.dataSize	= ( unsigned short* )packetBuffer + 0;					//  packetSize size = 2
	packetHeader.protocolID = ( unsigned short* )( ( char* )packetBuffer + 2 );		//  protocolID size	= 2

	//패킷헤더 뒤로 이동한다
	dataField      = &packetBuffer[4];
	readPosition   = writePosition = dataField;

	//packetBuffer의 마지막 위치로 이동
	endOfDataField = &dataField[PACKETBUFFERSIZE - 1];

	id( 0 );

	receivedSize = 0;
}

PacketQueue& PacketQueue::id( unsigned short ID )
{
	*packetHeader.protocolID = ID;

	return *this;
}

unsigned short PacketQueue::id()
{
	return *packetHeader.protocolID;
}

unsigned short PacketQueue::getDataFieldSize()
{
	return *packetHeader.dataSize;
}

void PacketQueue::copyToBuffer( char* buff, int size )
{
	clear();
	::CopyMemory( packetBuffer, buff, size );
	receivedSize += size;
}

void PacketQueue::readData( void* buffer, int size )
{
	//readPosition을 읽을 데이터만큼 이동시켰을 때
	//dataField를 넘어서거나
	if( readPosition + size > dataField + getDataFieldSize() || 
	//packetBuffer의 최대크기를 넘어서면 
		readPosition + size > endOfDataField )
	//데이터를 써(Write)주지 않는다.
		return;

	::CopyMemory( buffer, readPosition, size );
	readPosition += size;
}

void PacketQueue::writeData( void* buffer, int size )
{
	if( writePosition + size > endOfDataField )
		return;

	::CopyMemory( writePosition, buffer, size );
	writePosition += size;
	
	//헤더 포함 데이터량 증가
	receivedSize += size;

	//순수 데이터량 증가
	*packetHeader.dataSize += size;
}


////////////////////////////////////////////////////////////////////////////
//			    				Operators								  //
////////////////////////////////////////////////////////////////////////////

PacketQueue& PacketQueue::operator = ( PacketQueue& packet )
{
	::CopyMemory( dataField, packet.dataField, packet.getDataFieldSize() );

	*packetHeader.protocolID = packet.id();
	*packetHeader.dataSize   = packet.getDataFieldSize();

	return *this;
}

PacketQueue& PacketQueue::operator << ( LPTSTR arg )
{
	writeData( arg, lstrlen( arg ) * sizeof( TCHAR ) + sizeof( TCHAR ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( LPTSTR arg )
{
	readData( arg, lstrlen( ( LPTSTR )readPosition ) * sizeof( TCHAR ) + sizeof( TCHAR ) );
	
	return *this;
}

PacketQueue& PacketQueue::operator << ( PacketQueue& arg )
{
	unsigned int idValue = arg.id();
	unsigned int size = arg.getDataFieldSize();

	writeData( &idValue, sizeof( unsigned int ) );
	writeData( &size, sizeof( unsigned int ) );
	writeData( arg.dataField, size );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( PacketQueue& arg )
{
	int idValue, size;
	char buffer[PACKETBUFFERSIZE];

	readData( &idValue, sizeof( int ) );
	readData( &size, sizeof( int ) );

	readData( buffer, size );

	arg.id( idValue );
	arg.writeData( buffer, size );

	return *this;
}

PacketQueue& PacketQueue::operator << ( bool arg )
{
	writeData( &arg, sizeof( bool ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( bool& arg )
{
	readData( &arg, sizeof( bool ) );

	return *this;
}


PacketQueue& PacketQueue::operator << ( int arg )
{
	writeData( &arg, sizeof( int ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( int& arg )
{
	readData( &arg, sizeof( int ) );

	return *this;
}

PacketQueue& PacketQueue::operator << ( long arg )
{
	writeData( &arg, sizeof( long ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( long& arg )
{
	readData( &arg, sizeof( long ) );

	return *this;
}

PacketQueue& PacketQueue::operator << ( DWORD arg )
{
	writeData( &arg, sizeof( DWORD ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( DWORD& arg )
{
	readData( &arg, sizeof( DWORD ) );

	return *this;
}

PacketQueue& PacketQueue::operator << ( __int64 arg )
{
	writeData( &arg, sizeof( __int64 ) );

	return *this;
}

PacketQueue& PacketQueue::operator >> ( __int64& arg )
{
	readData( &arg, sizeof( __int64 ) );

	return* this;
}
