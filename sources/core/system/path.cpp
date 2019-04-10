#include "path.h"

#include <Shlwapi.h>
#include <ShlObj.h>

sys::path::path( pcstr str )
{
	GetFullPathName( str, 256, m_string.data( ), nullptr );
}

pcstr sys::path::c_str( ) const
{
	return m_string.c_str( );
}

sys::path sys::path::operator+( pcstr str ) const
{
	path result;
	PathCombine( result.m_string.data( ), m_string.c_str( ), str );
	return result;
}

sys::path& sys::path::operator+=( pcstr str )
{
	PathAppend( m_string.data( ), str );
	return *this;
}

bool sys::path::is_valid( ) const
{
	return is_valid( m_string.c_str( ) );
}

pcstr sys::path::get_file_name( ) const
{
	return get_file_name( m_string.c_str( ) );
}

void sys::path::create_directory( ) const
{
	create_directory( m_string.c_str( ) );
}

void sys::path::remove_file_extension( )
{
	remove_file_extension( m_string.data( ) );
}

pcstr sys::path::get_file_extension( )
{
	return get_file_extension( m_string.c_str( ) );
}

bool sys::path::is_valid( pcstr in_path )
{
	return PathFileExists( in_path ) == TRUE;
}

pcstr sys::path::get_file_name( pcstr in_path )
{
	return PathFindFileName( in_path );
}

void sys::path::create_directory( pcstr in_path )
{
	int result = SHCreateDirectoryEx( nullptr, in_path, nullptr );
	ASSERT( ( result == ERROR_SUCCESS ) || ( result == ERROR_ALREADY_EXISTS ), "create directory error: \"%s\"\n", in_path );
}

void sys::path::remove_file_extension( pstr in_path )
{
	PathRemoveExtension( in_path );
}

pcstr sys::path::get_file_extension( pcstr in_path )
{
	return PathFindExtension( in_path );
}