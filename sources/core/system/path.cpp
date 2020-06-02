#include "path.h"

#include <Shlwapi.h>
#include <ShlObj.h>

sys::path::path( pcstr str ) :
	m_string( str )
{ }

pcstr sys::path::c_str( ) const
{
	return m_string.c_str( );
}

sys::path sys::path::operator+( pcstr str ) const
{
	// Do not built-in functions due to '.' and '..' corruption
	ASSERT( strings::length( str ) > 0 );

	path result;
	uptr l = m_string.length( );

	strings::copy_n( result.m_string.data( ), m_string.c_str( ), l );

	if ( ( m_string.c_str( )[l - 1] != '/' ) && ( m_string.c_str( )[l - 1] != '\\' ) && ( str[0] != '/' ) && ( str[0] != '\\' ) )
		result.m_string.data( )[l++] = '/';

	strings::copy( result.m_string.data( ) + l, str );

	return result;
}

sys::path& sys::path::operator+=( pcstr str )
{
	// Do not built-in functions due to '.' and '..' corruption
	ASSERT( strings::length( str ) > 0 );

	uptr l = m_string.length( );

	if ( ( m_string.c_str( )[l - 1] != '/' ) && ( m_string.c_str( )[l - 1] != '\\' ) && ( str[0] != '/' ) && ( str[0] != '\\' ) )
		m_string.data( )[l++] = '/';

	strings::copy( m_string.data( ) + l, str );

	return *this;
}

void sys::path::add_extension( pcstr in_extension )
{
	strings::copy( m_string.data( ) + m_string.length( ), in_extension );
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
	str1024 path;
	GetFullPathName( in_path, str1024::max_string_size, path.data( ), nullptr );
	int result = SHCreateDirectoryEx( nullptr, path.c_str( ), nullptr );
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

sys::path sys::path::rebase( pcstr in_path, uptr in_base_length, pcstr in_new_base )
{
	ASSERT( strings::length( in_path ) >= in_base_length );
	sys::path result = in_new_base;
	result += in_path + in_base_length;
	return result;
}