#ifndef GUARD_CORE_LINEAR_QUEUE_H_INCLUDED
#define GUARD_CORE_LINEAR_QUEUE_H_INCLUDED

#include <types.h>

namespace lib {

template<typename T>
struct linear_queue
{
public:
	typedef T value_type;

public:
	void create( pointer const memory, uptr const size );

	void push( T const& value );
	void pop( T& value );

	void clear( );

	T& front( );
	T const& front( ) const;

	T& back( );
	T const& back( ) const;

	bool empty( ) const;

protected:
	T*		m_data;
	uptr	m_size;

	uptr	m_push_index;
	uptr	m_pop_index;
};

} // namespace lib

#include "linear_queue_inline.h"

#endif // #ifndef GUARD_CORE_LINEAR_QUEUE_H_INCLUDED