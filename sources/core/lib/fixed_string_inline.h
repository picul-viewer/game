#ifndef GUARD_CORE_FIXED_STRING_INLINE_H_INCLUDED
#define GUARD_CORE_FIXED_STRING_INLINE_H_INCLUDED

#include <macros.h>
#include <cstring>
#include "algorithms.h"
#include "weak_string.h"
#include <lib/memory.h>

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( )
{
	*m_data = '\0';
}

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( pcstr str ) :
	fixed_string( weak_const_string( str ) )
{ }

template<uptr MaxSize>
template<typename StringClass>
fixed_string<MaxSize>::fixed_string( i_const_string<StringClass> const& str )
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	ASSERT_CMP( str_casted.length( ), <, MaxSize - 1 );
	strings::copy( m_data, str_casted.c_str( ) );
}

template<uptr MaxSize>
fixed_string<MaxSize>::fixed_string( pcstr str, uptr size )
{
	ASSERT( str );
	ASSERT_CMP( size, <, MaxSize - 1 );
	strings::copy_n( m_data, str, size );
	m_data[size] = '\0';
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator=( pcstr str )
{
	return *this = weak_const_string( str );
}

template<uptr MaxSize>
template<typename StringClass>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator=( i_const_string<StringClass> const& str )
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	ASSERT_CMP( str_casted.length( ), <, MaxSize - 1 );
	strings::copy( m_data, str_casted.c_str( ) );

	return *this;
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::assign( pcstr str )
{
	return *this = str;
}

template<uptr MaxSize>
template<typename StringClass>
fixed_string<MaxSize>& fixed_string<MaxSize>::assign( i_const_string<StringClass> const& str )
{
	return *this = str;
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator+=( pcstr str )
{
	return *this += weak_const_string( str );
}

template<uptr MaxSize>
template<typename StringClass>
fixed_string<MaxSize>& fixed_string<MaxSize>::operator+=( i_const_string<StringClass> const& str )
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	uptr l = length( );
	ASSERT_CMP( l + str_casted.length( ), <, MaxSize - 1 );
	strings::copy( m_data + l, str_casted.c_str( ) );
	return *this;
}

template<uptr MaxSize>
fixed_string<MaxSize>& fixed_string<MaxSize>::append( pcstr str )
{
	return *this += str;
}

template<uptr MaxSize>
template<typename StringClass>
fixed_string<MaxSize>& fixed_string<MaxSize>::append( i_const_string<StringClass> const& str )
{
	return *this += str;
}

template<uptr MaxSize>
pcstr fixed_string<MaxSize>::c_str( ) const
{
	return m_data;
}

template<uptr MaxSize>
pstr fixed_string<MaxSize>::data( )
{
	return m_data;
}

template<uptr MaxSize>
fixed_string<MaxSize> fixed_string<MaxSize>::copy( uptr start, uptr length ) const
{
	return fixed_string<MaxSize>( m_data + start, length );
}

template<uptr MaxSize>
fixed_string<MaxSize> operator+( fixed_string<MaxSize> const& l, pcstr r )
{
	return l + weak_const_string( r );
}

template<uptr MaxSize, typename StringClass>
fixed_string<MaxSize> operator+( fixed_string<MaxSize> const& l, i_const_string<StringClass> const& r )
{
	StringClass const& r_casted = static_cast<StringClass const&>( r );
	ASSERT( r_casted.c_str( ) );

	uptr const ll = l.length( );
	ASSERT_CMP( ll + r_casted.length( ), <, MaxSize - 1 );
	fixed_string<MaxSize> result;
	strings::copy_n( result.data( ), l.c_str( ), ll );
	strings::copy( result.data( ) + ll, r_casted.c_str( ) );
	return result;
}

template<typename StrType, typename ... Args>
StrType format( pcstr const mask, Args&& ... args )
{
	StrType result;
	snprintf( result.data( ), StrType::max_string_size, mask, lib::move( args ) ... );
	return result;
}

#endif // #ifndef GUARD_CORE_FIXED_STRING_INLINE_H_INCLUDED
