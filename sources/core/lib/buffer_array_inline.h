#ifndef __core_buffer_array_inline_h_included_
#define __core_buffer_array_inline_h_included_

#include <macros.h>

template<typename T>
buffer_array<T>::buffer_array( pointer buffer, uptr capacity, uptr size ) :
	m_begin			( buffer ),
	m_end			( m_begin + size )

#ifdef DEBUG
	, m_max_end		( m_begin + capacity )
#endif // #ifdef DEBUG
{ }

template<typename T>
void buffer_array<T>::set( pointer buffer, uptr capacity, uptr size )
{
	m_begin			= buffer;
	m_end			= m_begin + size;

#ifdef DEBUG
	m_max_end		= m_begin + capacity;
#endif // #ifdef DEBUG
}

template<typename T>
void buffer_array<T>::push_back( T const& value )
{
	emplace_back( )	= value;
}

template<typename T>
T& buffer_array<T>::emplace_back( )
{
	ASSERT_CMP		( m_end, <, m_max_end );
	return			*(m_end++);
}

template<typename T>
void buffer_array<T>::resize( uptr size )
{
	T* new_end		= m_begin + size;
	ASSERT_CMP		( new_end, <, m_max_end );
	m_end			= new_end;
}

template<typename T>
void buffer_array<T>::reserve( uptr size )
{
	T* new_end		= m_begin + size;
	ASSERT_CMP		( new_end, <, m_max_end );
	m_end			= math::max( m_end, new_end );
}

template<typename T>
void buffer_array<T>::clear( )
{
	m_end			= m_begin;
}

template<typename T>
uptr buffer_array<T>::size( ) const
{
	return m_end - m_begin;
}

template<typename T>
T* buffer_array<T>::data( ) const
{
	return begin( );
}

template<typename T>
T* buffer_array<T>::begin( ) const
{
	return m_begin;
}

template<typename T>
T* buffer_array<T>::end( ) const
{
	return m_end;
}

template<typename T>
T& buffer_array<T>::at( uptr index )
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T const& buffer_array<T>::at( uptr index ) const
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T& buffer_array<T>::operator[]( uptr index )
{
	return at( index );
}

template<typename T>
T const& buffer_array<T>::operator[]( uptr index ) const
{
	return at( index );
}

template<typename T>
template<typename Pred>
void buffer_array<T>::for_each( Pred const& functor ) const
{
	for ( T* i = m_begin; i != m_end; ++i )
		functor( i );
}

#endif // #ifndef __core_buffer_array_inline_h_included_