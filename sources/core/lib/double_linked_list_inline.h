#ifndef __core_double_linked_list_inline_h_included_
#define __core_double_linked_list_inline_h_included_

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

#endif // #ifndef __core_double_linked_list_inline_h_included_