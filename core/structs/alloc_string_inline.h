#ifndef __core_alloc_string_inline_h_included_
#define __core_alloc_string_inline_h_included_

#include <core/macros.h>
#include <cstring>

template<u32 HeapPageSize>
alloc_string::alloc_string( alloc_string_heap<HeapPageSize>& heap, const char* str )
{
	uptr l = strlen( str );
	m_data = (char*)heap.allocate_size( l + 1 );
	strcpy( m_data, str );
}

template<u32 HeapPageSize>
alloc_string::alloc_string( alloc_string_heap<HeapPageSize>& heap, const char* str, uptr size )
{
	m_data = (char*)heap.allocate_size( size + 1 );
	memcpy( m_data, str, size * sizeof(char) );
	m_data[size] = '\0';
}

template<u32 HeapPageSize>
alloc_string::alloc_string( alloc_string_heap<HeapPageSize>& heap, i_const_string const& str )
{
	uptr l = str.length( );
	m_data = (char*)heap.allocate_size( l + 1 );
	strcpy( m_data, str.c_str( ) );
}

#endif // #ifndef __core_alloc_string_inline_h_included_
