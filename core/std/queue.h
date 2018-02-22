#ifndef __core_queue_h_included_
#define __core_queue_h_included_

#include <core/types.h>

template<typename T>
struct linear_queue
{
public:
	linear_queue( ) = default;

	void create( uptr size );
	void destroy( );

	void push( T const& value );
	void pop( T& value );
	
	T& front( );
	T const& front( ) const;

	T& back( );
	T const& back( ) const;

	bool empty( ) const;
	bool full( ) const;

protected:
	T*		m_data;
	uptr	m_size;

	uptr	m_push_index;
	uptr	m_pop_index;
};

#include "queue_inline.h"

#endif // #ifndef __core_queue_h_included_