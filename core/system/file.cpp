#include "file.h"

#include <io.h>
#include <fcntl.h>

namespace sys {

file::file( ) :
	m_handle( -1 )
{ }

file::file( path const& p, open_mode mode )
{
	int open_flag = 0;

	switch ( mode )
	{
	case open_read:
		open_flag = _O_RDONLY;
		break;
	case open_write_create:
		open_flag = _O_WRONLY | _O_CREAT;
		break;
	case open_write_append:
		open_flag = _O_WRONLY | _O_APPEND;
		break;
	default:
		UNREACHABLE_CODE
	}

	m_handle = _open( p.get_string( ).c_str( ), open_flag );

	_lseeki64( m_handle, 0, SEEK_END );
	m_size = _telli64( m_handle );
	_lseeki64( m_handle, 0, SEEK_SET );
}

file::~file( )
{
	if ( is_valid( ) )
		close( );
}

void file::close( )
{
	ASSERT( m_handle > 0 );
	_close( m_handle );
	m_handle = -1;
}

uptr file::size( ) const
{
	return m_size;
}

void file::seek( ptr position, seek_mode mode )
{
	int origin = -1;

	switch ( mode )
	{
	case seek_from_begin:
		origin = SEEK_SET;
		break;
	case seek_from_current:
		origin = SEEK_CUR;
		break;
	case seek_from_end:
		origin = SEEK_END;
		break;
	default:
		UNREACHABLE_CODE
	}

	_lseeki64( m_handle, position, origin );
}

uptr file::read( pvoid buffer, uptr size )
{
	ASSERT( buffer );
	ASSERT( size <= 0x7FFFFFFF );

	return _read( m_handle, buffer, (u32)size );
}

uptr file::write( pvoid buffer, uptr size )
{
	ASSERT( buffer );
	ASSERT( size <= 0x7FFFFFFF );

	return _write( m_handle, buffer, (u32)size );
}

bool file::is_valid( ) const
{
	return m_handle >= 0;
}

}