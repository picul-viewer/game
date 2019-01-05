#include "file.h"

#include <Windows.h>

#include <macros.h>

namespace sys {

file::file( ) :
	m_handle( INVALID_HANDLE_VALUE )
{ }

file::file( pcstr path, open_mode mode )
{
	open( path, mode );
}

void file::open( pcstr path, open_mode mode )
{
	DWORD access_flags = 0;
	DWORD share_flags = 0;
	DWORD creation = 0;

	switch ( mode )
	{
	case open_read:
		access_flags	= GENERIC_READ;
		share_flags		= FILE_SHARE_READ;
		creation		= OPEN_EXISTING;
		break;
	case open_write:
		access_flags	= GENERIC_WRITE;
		creation		= CREATE_ALWAYS;
		break;
	default:
		UNREACHABLE_CODE
	}

	m_handle = CreateFile( path, access_flags, share_flags, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr );

	ASSERT( m_handle != INVALID_HANDLE_VALUE, "open file error: \"%s\"\nerror type: %d\n", path, GetLastError( ) );
}

file::~file( )
{
	if ( is_valid( ) )
		close( );
}

void file::close( )
{
	ASSERT( is_valid( ) );
	CloseHandle( m_handle );
	m_handle = INVALID_HANDLE_VALUE;
}

uptr file::size( ) const
{
	LARGE_INTEGER size;
	BOOL result = GetFileSizeEx( m_handle, &size );

	ASSERT( result == TRUE, "FS error: 0x%016d\n", GetLastError( ) );
	ASSERT_CMP( size.QuadPart, >=, 0 );

	return (uptr)size.QuadPart;
}

void file::seek( ptr position, seek_mode mode )
{
	DWORD origin = -1;

	switch ( mode )
	{
	case seek_from_begin:
		origin = FILE_BEGIN;
		break;
	case seek_from_current:
		origin = FILE_CURRENT;
		break;
	case seek_from_end:
		origin = FILE_END;
		break;
	default:
		UNREACHABLE_CODE
	}


	BOOL result = SetFilePointerEx( m_handle, *(LARGE_INTEGER*)&position, nullptr, origin );

	ASSERT( result == TRUE, "FS error: 0x%016d\n", GetLastError( ) );
}

void file::read( pvoid buffer, uptr size )
{
	ASSERT( buffer );
	ASSERT_CMP( size, <=, 0x7FFFFFFF );

	DWORD bytes_read;
	BOOL result = ReadFile( m_handle, buffer, (DWORD)size, &bytes_read, nullptr );
	
	ASSERT( result == TRUE, "FS error: 0x%016d\n", GetLastError( ) );
	ASSERT_CMP( bytes_read, ==, (DWORD)size );
}

void file::write( pvoid buffer, uptr size )
{
	ASSERT( buffer );
	ASSERT_CMP( size, <=, 0x7FFFFFFF );

	DWORD bytes_written;
	BOOL result = WriteFile( m_handle, buffer, (DWORD)size, &bytes_written, nullptr );

	ASSERT( result == TRUE, "FS error: 0x%016d\n", GetLastError( ) );
	ASSERT_CMP( bytes_written, ==, (DWORD)size );
}

bool file::is_valid( ) const
{
	return m_handle != INVALID_HANDLE_VALUE;
}

}