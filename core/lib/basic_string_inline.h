#ifndef __code_basic_string_inline_h_included_
#define __code_basic_string_inline_h_included_

#include <macros.h>
#include <cstring>

template<typename StringClass>
uptr i_const_string<StringClass>::length( ) const
{
	return strlen( c_str( ) );
}

template<typename StringClass>
bool i_const_string<StringClass>::empty( ) const
{
	return c_str( )[0] == '\0';
}

template<typename StringClass>
char i_const_string<StringClass>::operator[]( uptr index ) const
{
	ASSERT( index < length( ) );
	return c_str( )[index];
}

template<typename StringClass>
char i_const_string<StringClass>::at( uptr index ) const
{
	ASSERT( index < length( ) );
	return c_str( )[index];
}

template<typename StringClass>
inline char const* i_const_string<StringClass>::c_str( ) const
{
	return ((StringClass*)this)->c_str( );
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::find( const char* str, uptr pos ) const
{
	uptr l = length( );
	if ( pos >= l ) return (u32)-1;
	const char* find_result = strstr( c_str( ) + pos, str );
	return ( find_result != nullptr ) ? ( find_result - c_str( ) ) : (uptr)-1;
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::find( char c, uptr pos ) const
{
	const char* curr = c_str( );
	while ( curr != '\0' )
		if ( *curr == c ) return curr - c_str( );
	return (uptr)-1;
}


template<typename LStringClass, typename RStringClass>
bool operator==( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) == 0;
}

template<typename LStringClass, typename RStringClass>
bool operator!=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) != 0;
}

template<typename LStringClass, typename RStringClass>
bool operator<=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) <= 0;
}

template<typename LStringClass, typename RStringClass>
bool operator>=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) >= 0;
}

template<typename LStringClass, typename RStringClass>
bool operator<( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) < 0;
}

template<typename LStringClass, typename RStringClass>
bool operator>( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strcmp( left.c_str( ), right.c_str( ) ) > 0;
}


template<typename StringClass>
void i_string<StringClass>::clear( )
{
	data( )[0] = '\0';
}

template<typename StringClass>
void i_string<StringClass>::resize( uptr size )
{
	data( )[size] = '\0';
}

template<typename StringClass>
char& i_string<StringClass>::operator[]( uptr index )
{
	ASSERT( index < length( ) );
	return data( )[index];
}

template<typename StringClass>
char& i_string<StringClass>::at( uptr index )
{
	ASSERT( index < length( ) );
	return data( )[index];
}

template<typename StringClass>
inline char* i_string<StringClass>::data( ) const
{
	return ((StringClass*)this)->data( );
}

template<typename StringClass>
inline void i_string<StringClass>::replace( char value, char new_value )
{
	for ( uptr i = 0, l = length( ); i != l; ++i )
		if ( data( )[i] == value )
			data( )[i] = new_value;
}

#endif // #ifndef __code_basic_string_inline_h_included_