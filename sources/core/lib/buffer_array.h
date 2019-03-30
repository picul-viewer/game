#ifndef __core_buffer_array_h_included_
#define __core_buffer_array_h_included_

#include <types.h>

namespace lib {

template<typename T>
class buffer_array
{
public:
	void create( pointer buffer, uptr capacity, uptr size = 0 );

	void push_back( T const& value );
	T& emplace_back( );

	void resize( uptr size );
	void reserve( uptr size );
	void clear( );

	T* data( );
	T const* data( ) const;
	uptr size( ) const;

	T* begin( );
	T const* begin( ) const;
	T* end( );
	T const* end( ) const;

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

} // namespace lib

#include "buffer_array_inline.h"

#endif // #ifndef __core_buffer_array_h_included_