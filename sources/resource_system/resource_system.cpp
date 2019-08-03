#include "resource_system.h"
#include <lib/allocator.h>
#include <math/math_common.h>
#include <system/interlocked.h>
#include "queried_resources.h"
#include "resource_cook.h"

namespace resource_system {

struct resource_system::query_data
{
	pointer caller;
	union
	{
		struct
		{
			cook_functor functor;
			u32 thread_index;
		};
		struct
		{
			user_callback callback;
			pointer data;
			uptr data_size;
		};
	};
	u32 results_count;
	mt_u32 pending_count;

#pragma warning (push)
#pragma warning (disable:4200)
	query_result results[0];
#pragma warning (pop)

	static uptr calculate_size( u32 const in_results_count );
	uptr size( ) const;
};

uptr resource_system::query_data::calculate_size( u32 const in_results_count )
{
	return sizeof(query_data) + sizeof(query_result) * in_results_count;
}

uptr resource_system::query_data::size( ) const
{
	return calculate_size( results_count );
}


void resource_system::create( u32 const in_thread_count )
{
	m_thread_count = math::min( in_thread_count, (u32)engine_thread_count );
	m_helper_thread_count = m_thread_count - engine_helper_threads_first;

	{
		uptr const queue_size = 2 * 1024;
		uptr const helper_queue_size = 32 * 1024;

		uptr const queue_memory_size = queue_size * sizeof(query_data*);
		uptr const helper_queue_memory_size = helper_queue_size * sizeof(query_data*);

		uptr const memory_size = queue_memory_size * ( m_thread_count - ignore_thread_count ) + helper_queue_memory_size;

		pointer const memory = virtual_allocator( ).allocate( memory_size );

		pointer p = memory;

		for ( u32 i = 0; i < m_thread_count; ++i )
			if ( ( ( 1 << i ) & ignore_thread_mask ) == 0 )
			{
				m_queues[i].create( p, queue_memory_size );
				p += queue_memory_size;
			}

		m_helper_queue.create( p, helper_queue_memory_size );
	}

	m_temp_allocator.create( );
	
	u32 const cs_count = m_thread_count - engine_free_threads_first;
	for ( u32 i = 0; i < cs_count; ++i )
		m_cs[i].create( );

	for ( u32 i = 0; i < cv_max_count; ++i )
		m_cv[i].create( );

	m_keep_processing = true;
}

void resource_system::stop( )
{
	m_keep_processing = false;
	full_fence( );

	for ( u32 i = 0; i < engine_free_threads_count; ++i )
		m_cv[i].wake( );
	m_cv[engine_free_threads_count].wake_all( );
}

void resource_system::destroy( )
{
	u32 const cs_count = m_thread_count - engine_free_threads_first;
	for ( u32 i = 0; i < cs_count; ++i )
		m_cs[i].destroy( );

	for ( u32 i = 0; i < cv_max_count; ++i )
		m_cv[i].destroy( );

	m_temp_allocator.destroy( );

	pointer const queue_memory = m_queues[0].data( );

	for ( u32 i = 0; i < m_thread_count; ++i )
		m_queues[i].destroy( );
	m_helper_queue.destroy( );

	virtual_allocator( ).deallocate( queue_memory );
}

void resource_system::process_task( query_data* const data )
{
	ASSERT( m_temp_allocator.contains_pointer( data ) );
	ASSERT_CMP( data->pending_count, ==, 0 );

	queried_resources resources( data->results, data->results_count );

	if ( data->caller )
		data->functor( data->caller, resources );
	else
	{
		ASSERT( data->results_count != 0 );
		data->callback( resources, data->data, data->data_size );
	}

	uptr const data_size = data->size( );
	m_temp_allocator.deallocate( data, data_size );
}

void resource_system::busy_thread_job( u32 const in_thread_index, sys::time const in_time_limit )
{
	ASSERT_CMP( in_thread_index, <, engine_free_threads_first );

	ASSERT( ( ( 1 << in_thread_index ) & ignore_thread_mask ) == 0 );

	u32 const queue_index = in_thread_index;

	sys::time current;

	do
	{
		query_data* data;
		bool const pop_successful = m_queues[queue_index].pop( data );

		if ( !pop_successful )
			break;

		process_task( data );

		current = sys::time::now( );
	}
	while ( current < in_time_limit );
}

void resource_system::free_thread_job( u32 const in_thread_index )
{
	ASSERT_CMP( in_thread_index, >=, engine_free_threads_first );
	ASSERT_CMP( in_thread_index, <, engine_helper_threads_first );

	ASSERT( ( ( 1 << in_thread_index ) & ignore_thread_mask ) == 0 );

	u32 const queue_index = in_thread_index;
	u32 const event_index = in_thread_index - engine_free_threads_first;

	while ( m_keep_processing )
	{
		query_data* data;
		bool const pop_successful = m_queues[queue_index].pop( data );

		if ( pop_successful )
			process_task( data );
		else
		{
			m_cs[event_index].enter( );
			m_cv[event_index].sleep( m_cs[event_index] );
			m_cs[event_index].release( );
		}
	}
}

void resource_system::helper_thread_job( u32 const in_thread_index )
{
	ASSERT_CMP( in_thread_index, >=, engine_helper_threads_first );
	ASSERT_CMP( in_thread_index, <, engine_helper_threads_first + m_helper_thread_count );

	ASSERT( ( ( 1 << in_thread_index ) & ignore_thread_mask ) == 0 );

	u32 const queue_index = in_thread_index;
	u32 const cs_index = in_thread_index - engine_free_threads_first;
	u32 const cv_index = engine_free_threads_count;

	while ( m_keep_processing )
	{
		query_data* data;
		bool pop_successful;
		goto loop_start;

		do
		{
			process_task( data );
		loop_start:
			pop_successful = m_queues[queue_index].pop( data );
		}
		while ( pop_successful );
		
		pop_successful = m_helper_queue.pop( data );

		if ( pop_successful )
			process_task( data );
		else
		{
			m_cs[cs_index].enter( );
			m_cv[cv_index].sleep( m_cs[cs_index] );
			m_cs[cs_index].release( );
		}
	}
}

void resource_system::create_resources(
	query_info const* const in_queries,
	u32 const in_query_count,
	user_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size
)
{
	ASSERT( in_queries != nullptr );
	ASSERT( in_query_count != 0 );

	uptr const query_data_size = query_data::calculate_size( in_query_count );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->caller = nullptr;
	data->callback = in_callback;
	data->data = in_callback_data;
	data->data_size = in_callback_data_size;
	data->results_count = in_query_count;
	data->pending_count = in_query_count;

	push_cook_queries( in_queries, in_query_count, data );
}

void resource_system::destroy_resource(
	query_info const& in_query
)
{
	uptr const query_data_size = query_data::calculate_size( 0 );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->caller = in_query.this_ptr;
	data->functor = in_query.task.functor;
	data->thread_index = in_query.task.thread_index;
	data->results_count = 0;
	data->pending_count = 0;

	push_query_data( data );
}

void resource_system::create_child_resources(
	query_info const* const in_queries,
	u32 const in_query_count,
	query_info const& in_callback_info
)
{
	ASSERT( in_queries != nullptr );
	ASSERT( in_query_count != 0 );

	uptr const query_data_size = query_data::calculate_size( in_query_count );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->caller = in_callback_info.this_ptr;
	data->functor = in_callback_info.task.functor;
	data->thread_index = in_callback_info.task.thread_index;
	data->results_count = in_query_count;
	data->pending_count = in_query_count;

	push_cook_queries( in_queries, in_query_count, data );
}

void resource_system::execute_tasks(
	resource_cook* const in_cook,
	cook_task_info const* const in_tasks,
	u32 const in_task_count,
	cook_task_info const& in_callback
)
{
	ASSERT( in_tasks != nullptr );
	ASSERT( in_task_count != 0 );
	ASSERT( in_cook != nullptr );

	uptr const query_data_size = query_data::calculate_size( in_task_count );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->caller = in_cook;
	data->functor = in_callback.functor;
	data->thread_index = in_callback.thread_index;
	data->results_count = in_task_count;
	data->pending_count = in_task_count;

	u32 const query_data_offset = (u32)( (pointer)data - m_temp_allocator.data( ) );

	for ( u32 i = 0; i < in_task_count; ++i )
	{
		uptr const current_query_data_size = query_data::calculate_size( 0 );
		query_data* const current_data = m_temp_allocator.allocate( current_query_data_size );

		current_data->caller = in_cook;
		current_data->functor = in_tasks[i].functor;
		current_data->thread_index = in_tasks[i].thread_index;
		current_data->results_count = 0;
		current_data->pending_count = 0;

		push_query_data( current_data );
	}
}

void resource_system::push_cook_queries(
	query_info const* const in_queries,
	u32 const in_query_count,
	query_data* const in_query_data
)
{
	ASSERT( in_queries != nullptr );
	ASSERT( in_query_count != 0 );
	ASSERT( in_query_data != nullptr );

	u32 const query_data_offset = (u32)( (pointer)in_query_data - m_temp_allocator.data( ) );

	for ( u32 i = 0; i < in_query_count; ++i )
	{
		query_info const& info = in_queries[i];
		resource_cook* const cook = info.this_ptr;

		cook->parent_query( ) = query_data_offset;
		cook->result_index( ) = i;

		uptr const query_data_size = query_data::calculate_size( 0 );
		query_data* const data = m_temp_allocator.allocate( query_data_size );

		data->caller = cook;
		data->functor = info.task.functor;
		data->thread_index = info.task.thread_index;
		data->results_count = 0;
		data->pending_count = 0;

		push_query_data( data );
	}
}

void resource_system::push_query_data( query_data* const in_query_data )
{
	u32 const thread_index = in_query_data->caller ? in_query_data->thread_index : engine_thread_count;
	ASSERT_CMP( thread_index, <=, engine_thread_count );

	if ( thread_index == engine_thread_count )
	{
		m_helper_queue.push( in_query_data );
		
		m_cv[engine_free_threads_count].wake_all( );
	}
	else
	{
		ASSERT_CMP( thread_index, <, m_thread_count );
		ASSERT( ( ( 1 << thread_index ) & ignore_thread_mask ) == 0 );

		m_queues[thread_index].push( in_query_data );

		s32 const event_index = thread_index - engine_free_threads_first;
		if ( event_index >= 0 )
			m_cv[event_index].wake( );
	}
}

void resource_system::finish_cook( resource_cook* const in_cook, query_result const in_result )
{
	query_data* const data = m_temp_allocator.data( ) + in_cook->parent_query( );
	ASSERT( m_temp_allocator.contains_pointer( data ) );

	data->results[in_cook->result_index( )] = in_result;

	if ( interlocked_dec( data->pending_count ) == 0 )
		push_query_data( data );

	query_functor const destroyer = in_cook->m_destroyer;
	destroyer( in_cook );
}

resource_system g_resource_system;

} // namespace resource_system