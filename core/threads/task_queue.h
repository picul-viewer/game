#ifndef __core_task_queue_h_inlcuded_
#define __core_task_queue_h_inlcuded_

#include <core/types.h>
#include <core/std.h>
#include "thread.h"
#include "interlocked.h"

static const uptr c_task_queue_size				= 16 * Kb;

static const uptr c_max_task_parameters_size	= 256;

// Single producer, single consumer
class task_queue
{
public:
	typedef void(*functor)( va_list );

	void create( );
	void destroy( );

	template<typename ... Args>
	inline void push( void(*in_functor)( Args ... ), Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ) const, T const* this_ptr, Args ... args );

	void pop( functor& out_functor, va_list& out_args_list );
	
	bool empty( ) const;

	static void empty_func( va_list args );

protected:
	s8*									m_push_pointer;
	s8*									m_pop_pointer;

	fixed_array<s8, c_task_queue_size>	m_data;
	s8*									m_data_end;
};

// Single producer, single consumer
class thread_task_queue
{
public:
	typedef void(*functor)( va_list );

	void create( thread* in_thread );
	void destroy( );

	template<typename ... Args>
	inline void push( void(*in_functor)( Args ... ), Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args );
	template<typename T, typename ... Args>
	inline void push( void(T::*in_functor)( Args ... ) const, T const* this_ptr, Args ... args );

	void pop( functor& out_functor, va_list& out_args_list );
	
	bool empty( ) const;

protected:
	s8*									m_push_pointer;
	s8*									m_pop_pointer;

	thread*								m_worker_thread;
	bool								m_thread_sleeping;

	fixed_array<s8, c_task_queue_size>	m_data;
	s8*									m_data_end;
};

#include "task_queue_inline.h"

#endif // #ifndef __core_task_queue_h_inlcuded_
