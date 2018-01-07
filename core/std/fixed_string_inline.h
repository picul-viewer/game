#ifndef __core_fixed_string_inline_h_included_
#define __core_fixed_string_inline_h_included_

#include <core/macros.h>
#include <cstring>

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( )
{
	*m_data = '\0';
}

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( const char* str )
{
	ASSERT( strlen( str ) < MaxSize - 1 );
	strcpy( m_data, str );
}

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( const char* str, uptr size )
{
	ASSERT( size < MaxSize - 1 );
	strncpy( m_data, str, size );
	m_data[size] = '\0';
}

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( i_const_string const& str )
{
	ASSERT( str.length( ) < MaxSize - 1 );
	strcpy( m_data, str.c_str( ) );
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator=( i_const_string const& str )
{
	ASSERT( str.length( ) < MaxSize - 1 );
	strcpy( m_data, str.c_str( ) );
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::assign( i_const_string const& str )
{
	*this = str;
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator+=( i_const_string const& str )
{
	uptr l = length( );
	ASSERT( l + str.length( ) < MaxSize - 1 );
	strcpy( m_data + l, str.c_str( ) );
	return *this;
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::append( i_const_string const& str )
{
	*this += str;
}

template<uptr MaxSize>
char* fixed_string<MaxSize>::data( ) const
{
	return (char*)m_data;
}

template<uptr MaxSize>
const char* fixed_string<MaxSize>::c_str( ) const
{
	return m_data;
}

template<uptr MaxSize>
fixed_string<MaxSize> const& fixed_string<MaxSize>::copy( uptr start, uptr length ) const
{
	return fixed_string<MaxSize>( m_data + start, length );
}

template<uptr MaxSize>
fixed_string<MaxSize> operator+( fixed_string<MaxSize> const& l, i_const_string const& r )
{
	uptr ll = l.length( ), rl = r.length( );
	ASSERT( ll + rl < MaxSize - 1 );
	fixed_string<MaxSize> result;
	strcmp( result.data( ), l.data( ) );
	strcmp( result.data( ) + ll, r.c_str( ) );
	result.data( )[ll + rl] = '\0';
	return result;
}

#endif // #ifndef __core_fixed_string_inline_h_included_
