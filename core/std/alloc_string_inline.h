#ifndef __core_alloc_string_inline_h_included_
#define __core_alloc_string_inline_h_included_

#include <core/macros.h>
#include <cstring>

template<typename AllocatorType>
alloc_string::alloc_string( AllocatorType& allocator, const char* str )
{
	uptr l = strlen( str );
	m_data = allocator.allocate( l + 1 );
	strcpy( m_data, str );
}

template<typename AllocatorType>
alloc_string::alloc_string( AllocatorType& allocator, const char* str, uptr size )
{
	m_data = (char*)allocator.allocate( size + 1 );
	memcpy( m_data, str, size * sizeof(char) );
	m_data[size] = '\0';
}

template<typename AllocatorType>
alloc_string::alloc_string( AllocatorType& allocator, i_const_string const& str )
{
	uptr l = str.length( );
	m_data = (char*)allocator.allocate( l + 1 );
	strcpy( m_data, str.c_str( ) );
}

#endif // #ifndef __core_alloc_string_inline_h_included_
