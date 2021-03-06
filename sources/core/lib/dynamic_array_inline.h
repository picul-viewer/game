#ifndef GUARD_CORE_DYNAMIC_ARRAY_INLINE_H_INCLUDED
#define GUARD_CORE_DYNAMIC_ARRAY_INLINE_H_INCLUDED

#include <macros.h>
#include <math/math_common.h>
#include "allocator.h"

template<typename T>
void lib::dynamic_array<T>::create( pointer const in_reserved_memory, uptr const capacity, uptr const size )
{
	static_assert					( sizeof(T) <= Memory_Page_Size, "too big object size" );

	if ( in_reserved_memory )
		m_begin						= in_reserved_memory;
	else
	{
		uptr const capacity_bytes	= math::align_up( capacity * sizeof(T), Memory_Page_Size );
		m_begin						= virtual_allocator( ).reserve( nullptr, capacity_bytes );
	}

	uptr const size_bytes			= math::align_up( size * sizeof(T), Memory_Page_Size );
	if ( size_bytes != 0 )
		virtual_allocator( ).commit	( m_begin, size_bytes );

	m_end							= m_begin + size;
	m_max_end						= m_begin + capacity;
}

template<typename T>
void lib::dynamic_array<T>::destroy( )
{
	virtual_allocator( ).release( m_begin );
}

template<typename T>
void lib::dynamic_array<T>::push_back( T const& value )
{
	emplace_back( )					= value;
}

template<typename T>
T& lib::dynamic_array<T>::emplace_back( )
{
	ASSERT_CMP						( m_end, <, m_max_end );

	T* const pointer_to_assign		= m_end;
	++m_end;

	if ( ( (uptr)m_end % Memory_Page_Size ) <= sizeof(T) )
		virtual_allocator( ).commit( pointer( m_end ).align_down( Memory_Page_Size ), Memory_Page_Size );

	return *pointer_to_assign;
}

template<typename T>
void lib::dynamic_array<T>::resize( uptr const size )
{
	T* new_end						= m_begin + size;
	ASSERT_CMP						( new_end, <=, m_max_end );

	uptr const size_allocated		= math::align_up( this->size( ) * sizeof(T), Memory_Page_Size );
	uptr const size_needed			= math::align_up( size * sizeof(T), Memory_Page_Size );

	if ( size_allocated > size_needed )
		virtual_allocator( ).decommit( (pointer)m_begin + size_needed, size_allocated - size_needed );
	else if ( size_allocated < size_needed )
		virtual_allocator( ).commit( (pointer)m_begin + size_allocated, size_needed - size_allocated );

	m_end							= new_end;
}

template<typename T>
void lib::dynamic_array<T>::reserve( uptr const size )
{
	T* new_end							= m_begin + size;
	ASSERT_CMP							( new_end, <=, m_max_end );

	if ( new_end > m_end )
	{
		uptr const size_allocated		= math::align_up( this->size( ) * sizeof(T), Memory_Page_Size );
		uptr const size_needed			= math::align_up( size * sizeof(T), Memory_Page_Size );

		if ( size_allocated != size_needed )
			virtual_allocator( ).commit	( (pointer)m_begin + size_allocated, size_needed - size_allocated );

		m_end							= new_end;
	}
}

template<typename T>
void lib::dynamic_array<T>::clear( )
{
	uptr const size_allocated		= math::align_up( ( m_end - m_begin ) * sizeof(T), Memory_Page_Size );
	
	virtual_allocator( ).decommit	( m_begin, size_allocated );

	m_end = m_begin;
}

template<typename T>
T* lib::dynamic_array<T>::data( )
{
	return begin( );
}

template<typename T>
T const* lib::dynamic_array<T>::data( ) const
{
	return begin( );
}

template<typename T>
uptr lib::dynamic_array<T>::size( ) const
{
	return m_end - m_begin;
}

template<typename T>
T* lib::dynamic_array<T>::begin( )
{
	return m_begin;
}

template<typename T>
T const* lib::dynamic_array<T>::begin( ) const
{
	return m_begin;
}

template<typename T>
T* lib::dynamic_array<T>::end( )
{
	return m_end;
}

template<typename T>
T const* lib::dynamic_array<T>::end( ) const
{
	return m_end;
}

template<typename T>
T& lib::dynamic_array<T>::at( uptr const index )
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T const& lib::dynamic_array<T>::at( uptr const index ) const
{
	ASSERT_CMP( m_begin + index, <, m_end );
	return m_begin[index];
}

template<typename T>
T& lib::dynamic_array<T>::operator[]( uptr const index )
{
	return at( index );
}

template<typename T>
T const& lib::dynamic_array<T>::operator[]( uptr const index ) const
{
	return at( index );
}

#endif // #ifndef GUARD_CORE_DYNAMIC_ARRAY_INLINE_H_INCLUDED