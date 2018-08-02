#include "strings.h"

#include <macros.h>
#include <cstring>

namespace strings {
	
uptr length( pcstr str )
{
	ASSERT( str );

	return strlen( str );
}

bool empty( pcstr str )
{
	ASSERT( str );

	return str[0] == '\0';
}

uptr find( pcstr str, pcstr sub_str, uptr pos )
{
	ASSERT( str );
	ASSERT( sub_str );
	
	ASSERT( pos < length( str ) );
	pcstr find_result = strstr( str + pos, str );
	return ( find_result != nullptr ) ? ( find_result - str ) : (uptr)-1;
}

uptr find( pcstr str, char c, uptr pos )
{
	ASSERT( str );
	
	for ( pcstr curr = str; curr != '\0'; ++curr )
		if ( *curr == c ) return curr - str;
	return (uptr)-1;
}

uptr rfind( pcstr str, char c )
{
	ASSERT( str );
	
	pcstr curr = str;
	ptr i = length( str ) - 1;
	for ( ; i >= 0; --i )
		if ( curr[i] == c ) return i;
	return (uptr)-1;
}

uptr rfind( pcstr str, char c, uptr pos )
{
	ASSERT( str );
	
	pcstr curr = str;
	ptr i = pos;
	for ( ; i >= 0; --i )
		if ( curr[i] == c ) return i;
	return (uptr)-1;
}

int compare( pcstr l, pcstr r )
{
	ASSERT( l );
	ASSERT( r );

	return strcmp( l, r );
}

bool equal( pcstr l, pcstr r )
{
	return compare( l, r ) == 0;
}

bool less( pcstr l, pcstr r )
{
	return compare( l, r ) < 0;
}

bool greater( pcstr l, pcstr r )
{
	return compare( l, r ) > 0;
}

}