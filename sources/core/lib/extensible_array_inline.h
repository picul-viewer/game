#ifndef __core_extensible_array_inline_h_included_
#define __core_extensible_array_inline_h_included_

#include <macros.h>
#include "allocator.h"

template<typename T>
void extensible_array<T>::create( uptr capacity, uptr size )
{
	uptr const capacity_in_bytes	= capacity * sizeof(T) + Memory_Page_Size - 1;
	uptr const capacity_to_allocate	= capacity_in_bytes - capacity_in_bytes % Memory_Page_Size;

	uptr const size_in_bytes		= size * sizeof(T) + Memory_Page_Size - 1;
	uptr const size_to_allocate		= size_in_bytes - size_in_bytes % Memory_Page_Size;

	m_begin							= virtual_allocator( ).reserve( nullptr, capacity_to_allocate );
	virtual_allocator( ).commit	( m_begin, size_to_allocate );

	m_end							= m_begin + size;
	m_max_end						= m_begin + capacity;
}

template<typename T>
void extensible_array<T>::destroy( )
{
	virtual_allocator( ).release( m_begin );
}

template<typename T>
void extensible_array<T>::push_back( T const& value )
{
	emplace_back( )					= value;
}

template<typename T>
T& extensible_array<T>::emplace_back( )
{
	ASSERT							( m_end < m_max_end );

	T* const pointer_to_assign		= m_end;
	++m_end;

	if ( ( (uptr)m_end % Memory_Page_Size - 1 ) < sizeof(T) )
		virtual_allocator( ).commit( (pvoid)( (uptr)m_end - (uptr)m_end % Memory_Page_Size ), Memory_Page_Size );

	return *pointer_to_assign;
}

template<typename T>
void extensible_array<T>::resize( uptr size )
{
	T* new_end						= m_begin + size;
	ASSERT							( new_end <= m_max_end );

	uptr const size_in_bytes		= this->size( ) * sizeof(T) + Memory_Page_Size - 1;
	uptr const size_allocated		= size_in_bytes - size_in_bytes % Memory_Page_Size;

	uptr const new_size_in_bytes	= size * sizeof(T) + Memory_Page_Size - 1;
	uptr const size_needed			= new_size_in_bytes - new_size_in_bytes % Memory_Page_Size;

	if ( size_allocated > size_needed )
		virtual_allocator( ).decommit( (pointer)m_begin + size_needed, size_allocated - size_needed );
	else if ( size_allocated < size_needed )
		virtual_allocator( ).commit( (pointer)m_begin + size_allocated, size_needed - size_allocated );

	m_end							= new_end;
}

template<typename T>
void extensible_array<T>::reserve( uptr size )
{
	T* new_end						= m_begin + size;
	ASSERT							( new_end <= m_max_end );

	if ( new_end > m_end )
	{
		uptr const size_in_bytes		= this->size( ) * sizeof(T) + Memory_Page_Size - 1;
		uptr const size_allocated		= size_in_bytes - size_in_bytes % Memory_Page_Size;

		uptr const new_size_in_bytes	= size * sizeof(T) + Memory_Page_Size - 1;
		uptr const size_needed			= new_size_in_bytes - new_size_in_bytes % Memory_Page_Size;

		if ( size_allocated != size_needed )
			virtual_allocator( ).commit( (pointer)m_begin + size_allocated, size_needed - size_allocated );

		m_end							= new_end;
	}
}

template<typename T>
void extensible_array<T>::clear( )
{
	uptr const size_in_bytes		= ( m_end - m_begin ) * sizeof(T) + Memory_Page_Size - 1;
	uptr const size_allocated		= size_in_bytes - size_in_bytes % Memory_Page_Size;
	
	virtual_allocator( ).decommit( m_begin, size_allocated );

	m_end = m_begin;
}

template<typename T>
uptr extensible_array<T>::size( ) const
{
	return m_end - m_begin;
}

template<typename T>
T* extensible_array<T>::begin( ) const
{
	return m_begin;
}

template<typename T>
T* extensible_array<T>::end( ) const
{
	return m_end;
}

template<typename T>
T& extensible_array<T>::at( uptr index )
{
	ASSERT( m_begin + index < m_end );
	return m_begin[index];
}

template<typename T>
T const& extensible_array<T>::at( uptr index ) const
{
	ASSERT( m_begin + index < m_end );
	return m_begin[index];
}

template<typename T>
T& extensible_array<T>::operator[]( uptr index )
{
	return at( index );
}

template<typename T>
T const& extensible_array<T>::operator[]( uptr index ) const
{
	return at( index );
}

template<typename T>
template<typename Pred>
void extensible_array<T>::for_each( Pred const& functor ) const
{
	for ( T* i = m_begin; i != m_end; ++i )
		functor( i );
}

#endif // #ifndef __core_extensible_array_inline_h_included_