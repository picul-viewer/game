#ifndef __core_queue_inline_h_included_
#define __core_queue_inline_h_included_

#include "memory.h"

template<typename T>
void linear_queue<T>::create( uptr size )
{
	m_data			= aligned_std_allocator<Cache_Line>( ).allocate( size );
	m_size			= size;

	m_push_index	= 0;
	m_pop_index		= 0;
}

template<typename T>
void linear_queue<T>::destroy( )
{
	aligned_std_allocator<Cache_Line>( ).deallocate( m_data );
}

template<typename T>
void linear_queue<T>::push( T const& value )
{
	ASSERT( !full( ) );

	m_data[m_push_index % m_size] = value;

	++m_push_index;
}

template<typename T>
void linear_queue<T>::pop( T& value )
{
	ASSERT( !empty( ) );

	value = m_data[( m_pop_index++ ) % m_size];
}

template<typename T>
T& linear_queue<T>::front( )
{
	ASSERT( !empty( ) );

	return m_data[m_pop_index % m_size];
}

template<typename T>
T const& linear_queue<T>::front( ) const
{
	ASSERT( !empty( ) );

	return m_data[m_pop_index % m_size];
}

template<typename T>
T& linear_queue<T>::back( )
{
	ASSERT( !empty( ) );

	return m_data[m_push_index % m_size];
}

template<typename T>
T const& linear_queue<T>::back( ) const
{
	ASSERT( !empty( ) );

	return m_data[m_push_index % m_size];
}

template<typename T>
bool linear_queue<T>::empty( ) const
{
	return m_push_index - m_pop_index <= 0;
}

template<typename T>
bool linear_queue<T>::full( ) const
{
	return m_push_index - m_pop_index >= m_size;
}

#endif // #ifndef __core_queue_inline_h_included_