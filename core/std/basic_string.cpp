#include "basic_string.h"

#include <core/macros.h>
#include <cstring>

uptr i_const_string::length( ) const
{
	return strlen( c_str( ) );
}

bool i_const_string::empty( ) const
{
	return c_str( )[0] == '\0';
}

char i_const_string::operator[]( uptr index ) const
{
	ASSERT( index < length( ) );
	return c_str( )[index];
}

char i_const_string::at( uptr index ) const
{
	ASSERT( index < length( ) );
	return c_str( )[index];
}

inline uptr i_const_string::find( const char* str, uptr pos ) const
{
	uptr l = length( );
	if ( pos >= l ) return (u32)-1;
	const char* find_result = strstr( c_str( ) + pos, str );
	return ( find_result != nullptr ) ? ( find_result - c_str( ) ) : (uptr)-1;
}

inline uptr i_const_string::find( char c, uptr pos ) const
{
	const char* curr = c_str( );
	while ( curr != '\0' )
		if ( *curr == c ) return curr - c_str( );
	return (uptr)-1;
}


bool operator==( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) == 0;
}

bool operator!=( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) != 0;
}

bool operator<=( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) <= 0;
}

bool operator>=( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) >= 0;
}

bool operator<( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) < 0;
}

bool operator>( i_const_string const& left, i_const_string const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) > 0;
}


void i_string::clear( )
{
	data( )[0] = '\0';
}

char& i_string::operator[]( uptr index )
{
	ASSERT( index < length( ) );
	return data( )[index];
}

char& i_string::at( uptr index )
{
	ASSERT( index < length( ) );
	return data( )[index];
}