#include "file_iterator.h"
#include <macros.h>
#include <lib/strings.h>

#include <Shlwapi.h>

void sys::file_iterator::create( pcstr mask )
{
	static_assert( find_data_size == sizeof(WIN32_FIND_DATA), "revisit find data size value in header" );

	m_handle = FindFirstFile( mask, (WIN32_FIND_DATA*)m_find_data );

	bool result = m_handle != INVALID_HANDLE_VALUE;

	while ( result && ( strings::equal( get_file_name( ), "." ) || strings::equal( get_file_name( ), ".." ) ) )
	{
		result = FindNextFile( m_handle, (WIN32_FIND_DATA*)m_find_data );
	}

	if ( !result )
	{
		FindClose( m_handle );
		m_handle = INVALID_HANDLE_VALUE;
	}
}

void sys::file_iterator::destroy( )
{
	if ( is_valid( ) )
	{
		FindClose( m_handle );
		m_handle = INVALID_HANDLE_VALUE;
	}
}

bool sys::file_iterator::next( )
{
	bool result = false;
	
	do 
	{
		result = FindNextFile( m_handle, (WIN32_FIND_DATA*)m_find_data );
	}
	while ( result && ( strings::equal( get_file_name( ), "." ) || strings::equal( get_file_name( ), ".." ) ) );

	return result;
}

bool sys::file_iterator::is_valid( ) const
{
	return m_handle != INVALID_HANDLE_VALUE;
}

bool sys::file_iterator::is_directory( ) const
{
	return ( (WIN32_FIND_DATA*)m_find_data )->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

pcstr sys::file_iterator::get_file_name( ) const
{
	return ( (WIN32_FIND_DATA*)m_find_data )->cFileName;
}

uptr sys::file_iterator::get_file_size( ) const
{
	return ( (WIN32_FIND_DATA*)m_find_data )->nFileSizeLow | ( (uptr)( (WIN32_FIND_DATA*)m_find_data )->nFileSizeLow << ( sizeof(DWORD) * 8 ) );
}

u64 sys::file_iterator::get_creation_time( ) const
{
	return *(u64*)&( (WIN32_FIND_DATA*)m_find_data )->ftCreationTime;
}

u64 sys::file_iterator::get_modification_time( ) const
{
	return *(u64*)&( (WIN32_FIND_DATA*)m_find_data )->ftLastWriteTime;
}