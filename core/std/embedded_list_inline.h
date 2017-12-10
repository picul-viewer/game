#ifndef __core_embedded_list_inline_h_included_
#define __core_embedded_list_inline_h_included_

template<typename T, T* (T::*Next)>
embedded_list<T, Next>::embedded_list( ) :
	m_first( nullptr )
{ }

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::insert( T* object )
{
	ASSERT( object );

	object->*Next	= m_first;
	m_first			= object;
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::remove( T* object )
{
	ASSERT( object );

	if ( object == m_first )
	{
		m_first		= m_first->*Next;
		return;
	}

	T* current		= m_first;
	ASSERT			( current );

	while ( current->*Next != object )
	{
		current		= current->*Next;
		ASSERT		( current );
	}
	current->*Next	= object->*Next;
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::clear( )
{
	m_first = nullptr;
}

template<typename T, T* (T::*Next)>
T* embedded_list<T, Next>::begin( ) const
{
	return m_first;
}

template<typename T, T* (T::*Next)>
T* embedded_list<T, Next>::end( ) const
{
	return nullptr;
}

template<typename T, T* (T::*Next)>
T* embedded_list<T, Next>::first( ) const
{
	return m_first;
}

template<typename T, T* (T::*Next)>
T* embedded_list<T, Next>::get_next( T* object ) const
{
	ASSERT( object );

	return object->*Next;
}

template<typename T, T* (T::*Next)>
template<typename Pred>
void embedded_list<T, Next>::for_each( Pred const& functor )
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}

template<typename T, T* (T::*Next)>
template<typename Pred>
void embedded_list<T, Next>::for_each( Pred const& functor ) const
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}


template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_list<T, Prev, Next>::insert( T* object )
{
	ASSERT( object );

	object->*Next		= m_first;
	if ( m_first )
		m_first->*Prev	= object;
	object->*Prev		= nullptr;
	m_first				= object;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_list<T, Prev, Next>::remove( T* object )
{
	ASSERT( object );

	if ( object->*Prev )
		object->*Prev->*Next	= object->*Next;
	else
		m_first					= object->*Next;

	if ( object->*Next )
		object->*Next->*Prev	= object->*Prev;
}


template<typename T, T* (T::*Prev), T* (T::*Next)>
embedded_twosided_reversable_list<T, Prev, Next>::embedded_twosided_reversable_list( ) :
	embedded_twosided_list	( ),
	m_last					( nullptr )
{ }

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::insert( T* object )
{
	ASSERT( object );

	object->*Next		= m_first;
	if ( m_first )
		m_first->*Prev	= object;
	else
		m_last			= object;
	object->*Prev		= nullptr;
	m_first				= object;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::remove( T* object )
{
	ASSERT( object );

	if ( object->*Prev )
		object->*Prev->*Next	= object->*Next;
	else
		m_first					= object->*Next;

	if ( object->*Next )
		object->*Next->*Prev	= object->*Prev;
	else
		m_last					= object->*Prev;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::clear( )
{
	m_first	= nullptr;
	m_last	= nullptr;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* embedded_twosided_reversable_list<T, Prev, Next>::rbegin( ) const
{
	return m_last;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* embedded_twosided_reversable_list<T, Prev, Next>::rend( ) const
{
	return nullptr;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* embedded_twosided_reversable_list<T, Prev, Next>::last( ) const
{
	return m_last;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
T* embedded_twosided_reversable_list<T, Prev, Next>::get_prev( T* object ) const
{
	return object->*Prev;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
template<typename Pred>
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( Pred const& functor )
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
template<typename Pred>
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( Pred const& functor ) const
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}


embedded_typeless_list::embedded_typeless_list( ) :
	m_first( nullptr )
{ }

void embedded_typeless_list::insert( pointer object )
{
	ASSERT( object );

	object.get<node>( ).next	= m_first;
	m_first						= object;
}

void embedded_typeless_list::remove( pointer object )
{
	ASSERT( object );

	if ( object == m_first )
	{
		m_first					= m_first.get<node>( ).next;
		return;
	}

	pointer current				= m_first;
	ASSERT						( current );

	while ( current.get<node>( ).next != object )
	{
		current					= current.get<node>( ).next;
		ASSERT					( current );
	}
	current.get<node>( ).next	= object.get<node>( ).next;
}

void embedded_typeless_list::clear( )
{
	m_first = nullptr;
}

pointer embedded_typeless_list::begin( ) const
{
	return m_first;
}

pointer embedded_typeless_list::end( ) const
{
	return nullptr;
}

pointer embedded_typeless_list::first( ) const
{
	return m_first;
}

pointer embedded_typeless_list::get_next( pointer object ) const
{
	ASSERT( object );

	return object.get<node>( ).next;
}

template<typename Pred>
void embedded_typeless_list::for_each( Pred const& functor )
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}

template<typename Pred>
void embedded_typeless_list::for_each( Pred const& functor ) const
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}


void embedded_typeless_twosided_list::insert( pointer object )
{
	ASSERT( object );

	object.get<node>( ).next		= m_first;
	if ( m_first )
		m_first.get<node>( ).prev	= object;
	object.get<node>( ).prev		= nullptr;
	m_first							= object;
}

void embedded_typeless_twosided_list::remove( pointer object )
{
	ASSERT( object );

	if ( object.get<node>( ).prev )
		object.get<node>( ).prev.get<node>( ).next	= object.get<node>( ).next;
	else
		m_first										= object.get<node>( ).next;

	if ( object.get<node>( ).next )
		object.get<node>( ).next.get<node>( ).prev	= object.get<node>( ).prev;
}


embedded_typeless_twosided_reversable_list::embedded_typeless_twosided_reversable_list( ) :
	embedded_typeless_twosided_list	( ),
	m_last					( nullptr )
{ }

void embedded_typeless_twosided_reversable_list::insert( pointer object )
{
	ASSERT( object );

	object.get<node>( ).next		= m_first;
	if ( m_first )
		m_first.get<node>( ).prev	= object;
	else
		m_last						= object;
	object.get<node>( ).prev		= nullptr;
	m_first							= object;
}

void embedded_typeless_twosided_reversable_list::remove( pointer object )
{
	ASSERT( object );

	if ( object.get<node>( ).prev )
		object.get<node>( ).prev.get<node>( ).next	= object.get<node>( ).next;
	else
		m_first										= object.get<node>( ).next;

	if ( object.get<node>( ).next )
		object.get<node>( ).next.get<node>( ).prev	= object.get<node>( ).prev;
	else
		m_last										= object.get<node>( ).prev;
}

void embedded_typeless_twosided_reversable_list::clear( )
{
	m_first	= nullptr;
	m_last	= nullptr;
}

pointer embedded_typeless_twosided_reversable_list::rbegin( ) const
{
	return m_last;
}

pointer embedded_typeless_twosided_reversable_list::rend( ) const
{
	return nullptr;
}

pointer embedded_typeless_twosided_reversable_list::last( ) const
{
	return m_last;
}

pointer embedded_typeless_twosided_reversable_list::get_prev( pointer object ) const
{
	return object.get<node>( ).prev;
}

template<typename Pred>
void embedded_typeless_twosided_reversable_list::reverse_for_each( Pred const& functor )
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}

template<typename Pred>
void embedded_typeless_twosided_reversable_list::reverse_for_each( Pred const& functor ) const
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}

#endif // #ifndef __core_embedded_list_inline_h_included_
