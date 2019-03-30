#ifndef __core_buffer_array_h_included_
#define __core_buffer_array_h_included_

#include <types.h>

namespace lib {

template<typename T>
class buffer_array
{
public:
	typedef T value_type;

	void create( pointer const buffer, uptr const capacity, uptr const size = 0 );

	void push_back( T const& value );
	T& emplace_back( );

	void resize( uptr const size );
	void reserve( uptr const size );
	void clear( );

	T* data( );
	T const* data( ) const;
	uptr size( ) const;

	T* begin( );
	T const* begin( ) const;
	T* end( );
	T const* end( ) const;

	T& at( uptr const index );
	T const& at( uptr const index ) const;
	T& operator[]( uptr const index );
	T const& operator[]( uptr const index ) const;

private:
	T*	m_begin;
	T*	m_end;
	T*	m_max_end;
};

} // namespace lib

#include "buffer_array_inline.h"

#endif // #ifndef __core_buffer_array_h_included_