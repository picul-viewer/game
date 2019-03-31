#ifndef __core_reversable_double_linked_list_inline_h_included_
#define __core_reversable_double_linked_list_inline_h_included_

#include <macros.h>

template<typename T, typename Linker>
void lib::reversable_double_linked_list<T, Linker>::create( )
{
	clear					( );
}

template<typename T, typename Linker>
void lib::reversable_double_linked_list<T, Linker>::insert_front( T* const object )
{
	ASSERT					( object != nullptr );

	Linker::set_next		( object, m_first );
	Linker::set_prev		( object, nullptr );

	if ( m_first )
		Linker::set_prev	( m_first, object );
	else
		m_last				= object;

	m_first					= object;
}

template<typename T, typename Linker>
void lib::reversable_double_linked_list<T, Linker>::insert_back( T* const object )
{
	ASSERT					( object != nullptr );

	Linker::set_next		( object, nullptr );
	Linker::set_prev		( object, m_last );

	if ( m_last )
		Linker::set_next	( m_last, object );
	else
		m_first				= object;

	m_last					= object;
}

template<typename T, typename Linker>
void lib::reversable_double_linked_list<T, Linker>::remove( T* const object )
{
	ASSERT					( object != nullptr );

	T* const prev			= Linker::get_prev( object );
	T* const next			= Linker::get_next( object );

	if ( prev )
		Linker::set_next	( prev, next )
	else
	{
		ASSERT				( object == m_first );
		m_first				= next;
	}

	if ( next )
		Linker::set_prev	( next, prev );
	else
	{
		ASSERT				( object == m_last );
		m_last				= prev;
	}
}

template<typename T, typename Linker>
void lib::reversable_double_linked_list<T, Linker>::clear( )
{
	m_first					= nullptr;
	m_last					= nullptr;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::reverse_iterator lib::reversable_double_linked_list<T, Linker>::rbegin( )
{
	return m_last;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator lib::reversable_double_linked_list<T, Linker>::rbegin( ) const
{
	return m_last;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::reverse_iterator lib::reversable_double_linked_list<T, Linker>::rend( )
{
	return nullptr;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator lib::reversable_double_linked_list<T, Linker>::rend( ) const
{
	return nullptr;
}


template<typename T, typename Linker>
lib::reversable_double_linked_list<T, Linker>::reverse_iterator::reverse_iterator( T* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::reverse_iterator& lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::reverse_iterator lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator++( int )
{
	reverse_iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator==( reverse_iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator!=( reverse_iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T* lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator*( ) const
{
	return m_object;
}

template<typename T, typename Linker>
T* lib::reversable_double_linked_list<T, Linker>::reverse_iterator::operator->( ) const
{
	return m_object;
}


template<typename T, typename Linker>
lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::const_reverse_iterator( T const* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator& lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator++( int )
{
	const_reverse_iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator==( const_reverse_iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator!=( const_reverse_iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T const* lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator*( ) const
{
	return m_object;
}

template<typename T, typename Linker>
T const* lib::reversable_double_linked_list<T, Linker>::const_reverse_iterator::operator->( ) const
{
	return m_object;
}

#endif // #ifndef __core_reversable_double_linked_list_inline_h_included_