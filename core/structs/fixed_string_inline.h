#ifndef __core_fixed_string_inline_h_included_
#define __core_fixed_string_inline_h_included_

#include <core/macros.h>
#include <cstring>

template<uptr max_size>
fixed_string<max_size>::fixed_string( )
{
	*m_data = '\0';
}

template<uptr max_size>
fixed_string<max_size>::fixed_string( const char* str )
{
	ASSERT( strlen( str ) < max_size - 1 );
	strcpy( m_data, str );
}

template<uptr max_size>
fixed_string<max_size>::fixed_string( const char* str, uptr size )
{
	ASSERT( size < max_size - 1 );
	strncpy( m_data, str, size );
	m_data[size] = '\0';
}

template<uptr max_size>
fixed_string<max_size>::fixed_string( i_const_string const& str )
{
	ASSERT( str.length( ) < max_size - 1 );
	strcpy( m_data, str.c_str( ) );
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::operator=( const char* str )
{
	ASSERT( str.length( ) < max_size - 1 );
	strcpy( m_data, str );
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::assign( const char* str )
{
	*this = str;

}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::operator=( i_const_string const& str )
{
	ASSERT( str.length( ) < max_size - 1 );
	strcpy( m_data, str.c_str( ) );
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::assign( i_const_string const& str )
{
	*this = str;
}

template<uptr max_size>
fixed_string<max_size> const& fixed_string<max_size>::operator+( const char* str )
{
	uptr ll = length( ), rl = strlen( str );
	ASSERT( ll + rl < max_size - 1 );
	fixed_string<max_size> result;
	strcmp( result.m_data, m_data );
	strcmp( result.m_data + ll, str );
	m_data[ll + rl] = '\0';
	return result;
}

template<uptr max_size>
fixed_string<max_size> const& fixed_string<max_size>::operator+( i_const_string const& str )
{
	uptr ll = length( ), rl = str.length( );
	ASSERT( ll + rl < max_size - 1 );
	fixed_string<max_size> result;
	strcmp( result.m_data, m_data );
	strcmp( result.m_data + ll, str.c_str( ) );
	m_data[ll + rl] = '\0';
	return result;
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::operator+=( const char* str )
{
	uptr l = length( );
	ASSERT( l + strlen( str ) < max_size - 1 );
	strcpy( m_data + l, str );
	return *this;
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::append( const char* str )
{
	*this += str;
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::operator+=( i_const_string const& str )
{
	uptr l = length( );
	ASSERT( l + str.length( ) < max_size - 1 );
	strcpy( m_data + l, str.c_str( ) );
	return *this;
}

template<uptr max_size>
fixed_string<max_size>& fixed_string<max_size>::append( i_const_string const& str )
{
	*this += str;
}

template<uptr max_size>
char* fixed_string<max_size>::data( ) const
{
	return (char*)m_data;
}

template<uptr max_size>
const char* fixed_string<max_size>::c_str( ) const
{
	return m_data;
}

template<uptr max_size>
fixed_string<max_size> const& fixed_string<max_size>::copy( uptr start, uptr length ) const
{
	return fixed_string<max_size>( m_data + start, length );
}

#endif // #ifndef __core_fixed_string_inline_h_included_
