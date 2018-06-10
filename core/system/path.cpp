#include "path.h"
#include <Shlwapi.h>

namespace sys {

path::path( str256 const& str ) :
	m_string( str )
{
	
}

str256 const& path::get_string( ) const
{
	return m_string;
}

path::operator str256( )
{
	return m_string;
}

path path::operator+( path const& p )
{
	path result = *this;
	result += p;
	return result;
}

path& path::operator+=( path const& p )
{
	auto result = PathAppend( m_string.data( ), p.get_string( ).c_str( ) );
	ASSERT( result == TRUE );
	return *this;
}

bool path::is_valid( ) const
{
	return PathFileExists( m_string.c_str( ) ) == TRUE;
}

}