#ifndef GUARD_CORE_BUFFER_ARRAY_INLINE_H_INCLUDED
#define GUARD_CORE_BUFFER_ARRAY_INLINE_H_INCLUDED

#include <macros.h>

template<typename T>
void lib::buffer_array<T>::create( pointer const buffer, uptr const capacity, uptr const size )
{
	m_begin			= buffer;
	m_end			= m_begin + size;
	m_max_end		= m_begin + capacity;
}

template<typename T>
void lib::buffer_array<T>::push_back( T const& value )
{
	emplace_back( )	= value;
}

template<typename T>
T& lib::buffer_array<T>::emplace_back( )
{
	ASSERT_CMP		( m_end, <, m_max_end );
	return			*(m_end++);
}

template<typename T>
void lib::buffer_array<T>::resize( uptr const size )
{
	T* new_end		= m_begin + size;
	ASSERT_CMP		( new_end, <, m_max_end );
	m_end			= new_end;
}

template<typename T>
void lib::buffer_array<T>::reserve( uptr const size )
{
	T* new_end		= m_begin + size;
	ASSERT_CMP		( new_end, <, m_max_end );
	m_end			= math::max( m_end, new_end );
}

template<typename T>
void lib::buffer_array<T>::clear( )
{
	m_end			= m_begin;
}

template<typename T>
T* lib::buffer_array<T>::data( )
{
	return begin( );
}

template<typename T>
T const* lib::buffer_array<T>::data( ) const
{
	return begin( );
}

template<typename T>
uptr lib::buffer_array<T>::size( ) const
{
	return m_end - m_begin;
}

template<typename T>
T* lib::buffer_array<T>::begin( )
{
	return m_begin;
}

template<typename T>
T const* lib::buffer_array<T>::begin( ) const
{
	return m_begin;
}

template<typename T>
T* lib::buffer_array<T>::end( )
{
	return m_end;
}

template<typename T>
T const* lib::buffer_array<T>::end( ) const
{
	return m_end;
}

template<typename T>
T& lib::buffer_array<T>::at( uptr const index )
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T const& lib::buffer_array<T>::at( uptr const index ) const
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T& lib::buffer_array<T>::operator[]( uptr const index )
{
	return at( index );
}

template<typename T>
T const& lib::buffer_array<T>::operator[]( uptr const index ) const
{
	return at( index );
}

#endif // #ifndef GUARD_CORE_BUFFER_ARRAY_INLINE_H_INCLUDED