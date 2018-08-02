#ifndef __core_alloc_string_inline_h_included_
#define __core_alloc_string_inline_h_included_

#include <macros.h>
#include <cstring>
#include "weak_string.h"
#include <lib/memory.h>

template<typename AllocatorType>
alloc_string::alloc_string( AllocatorType& allocator, pcstr str ) :
	alloc_string( allocator, weak_const_string( str ) )
{ }

template<typename AllocatorType, typename StringClass>
alloc_string::alloc_string( AllocatorType& allocator, i_const_string<StringClass> const& str )
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	uptr const l = str_casted.length( );
	m_data = (pstr)allocator.allocate( ( l + 1 ) * sizeof(char) );
	memory::copy( m_data, str_casted.c_str( ), ( l + 1 ) * sizeof(char) );
}

template<typename AllocatorType>
alloc_string::alloc_string( AllocatorType& allocator, pcstr str, uptr size )
{
	ASSERT( str );

	m_data = (pstr)allocator.allocate( ( size + 1 ) * sizeof(char) );
	memory::copy( m_data, str, size * sizeof(char) );
	m_data[size] = '\0';
}

#endif // #ifndef __core_alloc_string_inline_h_included_
