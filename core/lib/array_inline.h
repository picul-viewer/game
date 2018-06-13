#ifndef __core_array_inline_h_included_
#define __core_array_inline_h_included_

#include <macros.h>

template<typename T, typename Tcref>
buffer_array<T, Tcref>::buffer_array( pointer buffer, uptr capacity, uptr size ) :
	m_begin		( buffer ),
	m_end		( buffer + size ),
	m_max_end	( buffer + capacity )
{ }

template<typename T, typename Tcref>
void buffer_array<T, Tcref>::set( pointer buffer, uptr capacity, uptr size )
{
	m_begin		= buffer;
	m_end		= buffer + size;
	m_max_end	= buffer + capacity;
}

template<typename T, typename Tcref>
void buffer_array<T, Tcref>::push_back( T const& value )
{
	ASSERT		( m_end < m_max_end );
	*(m_end++)	= value;
}

template<typename T, typename Tcref>
T* buffer_array<T, Tcref>::emplace_back( )
{
	ASSERT		( m_end < m_max_end );
	return		m_end++;
}

template<typename T, typename Tcref>
void buffer_array<T, Tcref>::resize( uptr size )
{
	T* new_end	= m_begin + size;
	ASSERT		( new_end < m_max_end );
	for ( T* p = m_end; p < new_end; ++p )
		new (p) T( );
	m_end		= new_end;
}

template<typename T, typename Tcref>
void buffer_array<T, Tcref>::reserve( uptr size )
{
	T* new_end	= m_begin + size;
	ASSERT		( new_end < m_max_end );
	m_end		= new_end;
}

template<typename T, typename Tcref>
void buffer_array<T, Tcref>::clear( )
{
	m_end		= m_begin;
}

template<typename T, typename Tcref>
uptr buffer_array<T, Tcref>::size( ) const
{
	return m_end - m_begin;
}

template<typename T, typename Tcref>
T* buffer_array<T, Tcref>::begin( ) const
{
	return m_begin;
}

template<typename T, typename Tcref>
T* buffer_array<T, Tcref>::end( ) const
{
	return m_end;
}

template<typename T, typename Tcref>
T& buffer_array<T, Tcref>::operator[]( uptr index )
{
	ASSERT( m_begin + index < m_end );
	return m_begin[index];
}

template<typename T, typename Tcref>
Tcref buffer_array<T, Tcref>::operator[]( uptr index ) const
{
	ASSERT( m_begin + index < m_end );
	return m_begin[index];
}

template<typename T, typename Tcref>
template<typename Pred>
void buffer_array<T, Tcref>::for_each( Pred const& functor ) const
{
	for ( T* i = m_begin; i != m_end; ++i )
		functor( i );
}

#endif // #ifndef __core_array_inline_h_included_