#ifndef GUARD_CORE_LINEAR_QUEUE_INLINE_H_INCLUDED
#define GUARD_CORE_LINEAR_QUEUE_INLINE_H_INCLUDED

#include "memory.h"

template<typename T>
void lib::linear_queue<T>::create( pointer const memory, uptr const size )
{
	m_data			= memory;
	m_size			= size;

	clear			( );
}

template<typename T>
void lib::linear_queue<T>::push( T const& value )
{
	ASSERT( m_push_index != m_pop_index - 1 );

	m_data[m_push_index] = value;

	m_push_index = ( m_push_index + 1 ) % m_size;
}

template<typename T>
void lib::linear_queue<T>::pop( T& value )
{
	ASSERT( !empty( ) );

	value = m_data[m_pop_index];

	m_pop_index = ( m_pop_index + 1 ) % m_size;
}

template<typename T>
void lib::linear_queue<T>::clear( )
{
	m_push_index	= 0;
	m_pop_index		= 0;
}

template<typename T>
T& lib::linear_queue<T>::front( )
{
	ASSERT( !empty( ) );

	return m_data[m_pop_index];
}

template<typename T>
T const& lib::linear_queue<T>::front( ) const
{
	ASSERT( !empty( ) );

	return m_data[m_pop_index];
}

template<typename T>
T& lib::linear_queue<T>::back( )
{
	ASSERT( !empty( ) );

	return m_data[m_push_index];
}

template<typename T>
T const& lib::linear_queue<T>::back( ) const
{
	ASSERT( !empty( ) );

	return m_data[m_push_index];
}

template<typename T>
bool lib::linear_queue<T>::empty( ) const
{
	return m_push_index == m_pop_index;
}

#endif // #ifndef GUARD_CORE_LINEAR_QUEUE_INLINE_H_INCLUDED