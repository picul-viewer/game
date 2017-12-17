#ifndef __core_task_queue_h_inlcuded_
#define __core_task_queue_h_inlcuded_

#include <core/types.h>
#include <core/std.h>
#include <core/threads.h>

// Single producer, single consumer
template<uptr RecordSize>
class task_queue
{
public:
	task_queue( ) = default;
	~task_queue( ) = default;

	typedef procedure<RecordSize> functor;

	void create( uptr in_buffer_size );
	void destroy( );

	template<typename ... Args>
	inline void push( void(*in_functor)( Args ... ), Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ) const, T const* this_ptr, Args ... args );

	void pop( functor& out_functor );
	
	bool empty( ) const;
	bool full( ) const;

protected:
	functor*		m_data;

	mt_u32			m_push_index;
	mt_u32			m_pop_index;

	u32				m_index_mask;

	threading_event	m_event;
};

#include "task_queue_inline.h"

#endif // #ifndef __core_task_queue_h_inlcuded_
