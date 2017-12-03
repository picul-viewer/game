#ifndef __core_array_h_included_
#define __core_array_h_included_

#include <core/types.h>

#include <array>

template<typename T, uptr Size>
class fixed_array : public std::array<T, Size>
{ };

template<typename T, typename Tcref = T const&>
class buffer_array
{
public:
	buffer_array( ) = default;
	buffer_array( pointer buffer, uptr capacity, uptr size = 0 );
	void set( pointer buffer, uptr capacity, uptr size = 0 );

	void push_back( T const& value );
	T* emplace_back( );
	
	void resize( uptr size );
	void reserve( uptr size );
	void clear( );

	uptr size( ) const;

	T* begin( ) const;
	T* end( ) const;

	T& operator[]( uptr index );
	Tcref operator[]( uptr index ) const;
	
	template<typename Pred>
	void for_each( Pred const& functor );
	template<typename Pred>
	void for_each( Pred const& functor ) const;

protected:
	T*	m_begin;
	T*	m_end;
	T*	m_max_end;
};

#include "array_inline.h"

#endif // #ifndef __core_array_h_included_