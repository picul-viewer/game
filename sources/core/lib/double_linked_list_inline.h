#ifndef GUARD_CORE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED
#define GUARD_CORE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED

#include <macros.h>

template<typename T, typename Linker>
void lib::double_linked_list<T, Linker>::insert_front( T* const object )
{
	ASSERT					( object != nullptr );

	Linker::set_next		( object, m_first );
	if ( m_first )
		Linker::set_prev	( m_first, object );

	Linker::set_prev		( object, nullptr );
	m_first					= object;
}

template<typename T, typename Linker>
void lib::double_linked_list<T, Linker>::insert_after( T* const object, T* const prev )
{
	ASSERT					( object != nullptr );
	ASSERT					( prev != nullptr );

	T* const next			= Linker::get_next( prev );

	Linker::set_prev		( object, prev );
	Linker::set_next		( object, next );

	if ( next )
		Linker::set_prev	( next, object );
	else
	{
		ASSERT				( m_last == prev );
		m_last				= object;
	}

	Linker::set_next		( prev, object );
}

template<typename T, typename Linker>
void lib::double_linked_list<T, Linker>::insert_before( T* const object, T* const next )
{
	ASSERT					( object != nullptr );
	ASSERT					( next != nullptr );

	T* const prev			= Linker::get_prev( next );

	Linker::set_prev		( object, prev );
	Linker::set_next		( object, next );
	Linker::set_prev		( next, object );

	if ( next )
		Linker::set_prev	( next, object );
	else
	{
		ASSERT				( m_last == prev );
		m_last				= object;
	}

	if ( prev )
		Linker::set_next	( prev, object );
	else
	{
		ASSERT				( m_first == next );
		m_first				= object;
	}
}

template<typename T, typename Linker>
void lib::double_linked_list<T, Linker>::remove( T* const object )
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
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator lib::double_linked_list<T, Linker>::begin( )
{
	return m_first;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator lib::double_linked_list<T, Linker>::begin( ) const
{
	return m_first;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator lib::double_linked_list<T, Linker>::end( )
{
	return nullptr;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator lib::double_linked_list<T, Linker>::end( ) const
{
	return nullptr;
}


template<typename T, typename Linker>
lib::double_linked_list<T, Linker>::iterator::iterator( T* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator& lib::double_linked_list<T, Linker>::iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_next( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator lib::double_linked_list<T, Linker>::iterator::operator++( int )
{
	iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_next( m_object );

	return temp;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator& lib::double_linked_list<T, Linker>::iterator::operator--( )
{
	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::iterator lib::double_linked_list<T, Linker>::iterator::operator--( int )
{
	iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::double_linked_list<T, Linker>::iterator::operator==( iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::double_linked_list<T, Linker>::iterator::operator!=( iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T& lib::double_linked_list<T, Linker>::iterator::operator*( ) const
{
	return *m_object;
}

template<typename T, typename Linker>
T* lib::double_linked_list<T, Linker>::iterator::operator->( ) const
{
	return m_object;
}


template<typename T, typename Linker>
lib::double_linked_list<T, Linker>::const_iterator::const_iterator( T const* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator& lib::double_linked_list<T, Linker>::const_iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_next( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator lib::double_linked_list<T, Linker>::const_iterator::operator++( int )
{
	const_iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_next( m_object );

	return temp;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator& lib::double_linked_list<T, Linker>::const_iterator::operator--( )
{
	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::double_linked_list<T, Linker>::const_iterator lib::double_linked_list<T, Linker>::const_iterator::operator--( int )
{
	const_iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_prev( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::double_linked_list<T, Linker>::const_iterator::operator==( const_iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::double_linked_list<T, Linker>::const_iterator::operator!=( const_iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T const& lib::double_linked_list<T, Linker>::const_iterator::operator*( ) const
{
	return *m_object;
}

template<typename T, typename Linker>
T const* lib::double_linked_list<T, Linker>::const_iterator::operator->( ) const
{
	return m_object;
}


template<typename T, T* (T::*Prev), T* (T::*Next)>
void lib::intrusive_double_linked_list_linker<T, Prev, Next>::set_prev( T* const object, T* const next )
{
	object->*Prev = next;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* lib::intrusive_double_linked_list_linker<T, Prev, Next>::get_prev( T* const object )
{
	return object->*Prev;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T const* lib::intrusive_double_linked_list_linker<T, Prev, Next>::get_prev( T const* const object )
{
	return object->*Prev;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void lib::intrusive_double_linked_list_linker<T, Prev, Next>::set_next( T* const object, T* const next )
{
	object->*Next = next;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* lib::intrusive_double_linked_list_linker<T, Prev, Next>::get_next( T* const object )
{
	return object->*Next;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T const* lib::intrusive_double_linked_list_linker<T, Prev, Next>::get_next( T const* const object )
{
	return object->*Next;
}

#endif // #ifndef GUARD_CORE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED