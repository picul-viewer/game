#ifndef __core_embedded_list_inline_h_included_
#define __core_embedded_list_inline_h_included_

template<typename T, T* (T::*Next)>
embedded_list<T, Next>::embedded_list( ) :
	m_first( nullptr )
{ }

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::insert( T* object )
{
	object->*Next	= m_first;
	m_first			= object;
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
	return object->*Next;
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::for_each( for_each_functor_type functor )
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::for_each( const_for_each_functor_type functor ) const
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		functor( i );
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::for_each( for_each_method_type functor )
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		( i->*functor )( );
}

template<typename T, T* (T::*Next)>
void embedded_list<T, Next>::for_each( const_for_each_method_type functor ) const
{
	for ( auto i = first( ); i != nullptr; i = get_next( i ) )
		( i->*functor )( );
}


template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_list<T, Prev, Next>::insert( T* object )
{
	object->*Next		= m_first;
	if ( m_first )
		m_first->*Prev	= object;
	object->*Prev		= nullptr;
	m_first				= object;
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_list<T, Prev, Next>::remove( T* object )
{
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
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( for_each_functor_type functor )
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( const_for_each_functor_type functor ) const
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		functor( i );
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( for_each_method_type functor )
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		( i->*functor )( );
}

template<typename T, T* (T::*Prev), T* (T::*Next)>
void embedded_twosided_reversable_list<T, Prev, Next>::reverse_for_each( const_for_each_method_type functor ) const
{
	for ( auto i = last( ); i != nullptr; i = get_prev( i ) )
		( i->*functor )( );
}

#endif // #ifndef __core_embedded_list_inline_h_included_
