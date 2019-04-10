#ifndef GUARD_CORE_LIST_INLINE_H_INCLUDED
#define GUARD_CORE_LIST_INLINE_H_INCLUDED

#include <macros.h>

template<typename T, typename Linker>
void lib::list<T, Linker>::create( )
{
	clear					( );
}

template<typename T, typename Linker>
void lib::list<T, Linker>::insert_front( T* const object )
{
	ASSERT					( object != nullptr );

	Linker::set_next		( object, m_first );
	m_first					= object;
}

template<typename T, typename Linker>
void lib::list<T, Linker>::insert_after( T* const object, T* const prev )
{
	ASSERT					( object != nullptr );
	ASSERT					( prev != nullptr );

	T* const next			= Linker::get_next( prev );

	Linker::set_next		( object, next );
	Linker::set_next		( prev, object );
}

template<typename T, typename Linker>
void lib::list<T, Linker>::remove( T* const object )
{
	ASSERT					( object != nullptr );

	if ( object == m_first )
	{
		m_first				= Linker::get_next( m_first );
		return;
	}

	T* current				= m_first;
	ASSERT					( current != nullptr, "list does not contain provided object" );

	T* next					= Linker::get_next( current );

	while ( next != object )
	{
		current				= next;
		ASSERT				( current != nullptr, "list does not contain provided object" );

		next				= Linker::get_next( current );
	}

	Linker::set_next		( current, Linker::get_next( object ) );
}

template<typename T, typename Linker>
void lib::list<T, Linker>::clear( )
{
	m_first					= nullptr;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::iterator lib::list<T, Linker>::begin( )
{
	return m_first;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::const_iterator lib::list<T, Linker>::begin( ) const
{
	return m_first;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::iterator lib::list<T, Linker>::end( )
{
	return nullptr;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::const_iterator lib::list<T, Linker>::end( ) const
{
	return nullptr;
}


template<typename T, typename Linker>
lib::list<T, Linker>::iterator::iterator( T* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::iterator& lib::list<T, Linker>::iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_next( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::iterator lib::list<T, Linker>::iterator::operator++( int )
{
	iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_next( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::list<T, Linker>::iterator::operator==( iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::list<T, Linker>::iterator::operator!=( iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T& lib::list<T, Linker>::iterator::operator*( ) const
{
	return *m_object;
}

template<typename T, typename Linker>
T* lib::list<T, Linker>::iterator::operator->( ) const
{
	return m_object;
}


template<typename T, typename Linker>
lib::list<T, Linker>::const_iterator::const_iterator( T const* const object )
{
	m_object = object;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::const_iterator& lib::list<T, Linker>::const_iterator::operator++( )
{
	ASSERT( m_object );
	m_object = Linker::get_next( m_object );
	return *this;
}

template<typename T, typename Linker>
typename lib::list<T, Linker>::const_iterator lib::list<T, Linker>::const_iterator::operator++( int )
{
	const_iterator const temp = *this;

	ASSERT( m_object );
	m_object = Linker::get_next( m_object );

	return temp;
}

template<typename T, typename Linker>
bool lib::list<T, Linker>::const_iterator::operator==( const_iterator const other ) const
{
	return m_object == other.m_object;
}

template<typename T, typename Linker>
bool lib::list<T, Linker>::const_iterator::operator!=( const_iterator const other ) const
{
	return m_object != other.m_object;
}

template<typename T, typename Linker>
T const& lib::list<T, Linker>::const_iterator::operator*( ) const
{
	return *m_object;
}

template<typename T, typename Linker>
T const* lib::list<T, Linker>::const_iterator::operator->( ) const
{
	return m_object;
}


template<typename T, T* (T::*Next)>
void lib::intrusive_list_linker<T, Next>::set_next( T* const object, T* const next )
{
	object->*Next = next;
}

template<typename T, T* (T::*Next)>
T* lib::intrusive_list_linker<T, Next>::get_next( T* const object )
{
	return object->*Next;
}

template<typename T, T* (T::*Next)>
T const* lib::intrusive_list_linker<T, Next>::get_next( T const* const object )
{
	return object->*Next;
}

#endif // #ifndef GUARD_CORE_LIST_INLINE_H_INCLUDED