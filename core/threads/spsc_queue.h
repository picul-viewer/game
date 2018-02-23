#ifndef __core_spsc_queue_h_inlcuded_
#define __core_spsc_queue_h_inlcuded_

#include <types.h>

#include <threads/interlocked.h>
#include <threads/threading_event.h>

// Single producer, single consumer
template<typename T, uptr RecordSize = sizeof(T)>
class spsc_queue_no_wait
{
public:
	spsc_queue_no_wait( ) = default;
	~spsc_queue_no_wait( ) = default;

	typedef T value_type;

	void create( uptr in_buffer_size );
	void destroy( );

	void push( value_type const& in_value );
	void pop( value_type& out_value );
	
	bool empty( ) const;
	bool full( ) const;

protected:
	mt_s32			m_push_index;
	s32				m_unused0[Cache_Line / sizeof(s32) - 1];

	mt_s32			m_pop_index;
	s32				m_unused1[Cache_Line / sizeof(s32) - 1];
	
	value_type*		m_data;

	s32				m_index_mask;
};

template<typename T, uptr RecordSize = sizeof(T)>
class spsc_queue : public spsc_queue_no_wait<T, RecordSize>
{
public:
	spsc_queue( ) = default;
	~spsc_queue( ) = default;

	void create( uptr in_buffer_size );
	void destroy( );

	void push( value_type const& in_value );
	void pop( value_type& out_value );
	
protected:
	typedef spsc_queue_no_wait<T, RecordSize> parent;

	threading_event	m_empty_event;
	threading_event	m_full_event;
};

#include "spsc_queue_inline.h"

#endif // #ifndef __core_spsc_queue_h_inlcuded_
