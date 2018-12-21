#ifndef __core_extensible_array_h_included_
#define __core_extensible_array_h_included_

#include <types.h>

template<typename T>
class extensible_array
{
public:
	void create( uptr capacity, uptr size = 0 );
	void destroy( );

	void push_back( T const& value );
	T& emplace_back( );
	
	void resize( uptr size );
	void reserve( uptr size );
	void clear( );

	uptr size( ) const;
	
	T* data( ) const;
	T* begin( ) const;
	T* end( ) const;
	
	T& at( uptr index );
	T const& at( uptr index ) const;
	T& operator[]( uptr index );
	T const& operator[]( uptr index ) const;
	
	template<typename Pred>
	void for_each( Pred const& functor ) const;

protected:
	T*	m_begin;
	T*	m_end;
	T*	m_max_end;
};

#include "extensible_array_inline.h"

#endif // #ifndef __core_extensible_array_h_included_