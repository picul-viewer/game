#include "resource_system.h"
#include <lib/allocator.h>
#include <math/math_common.h>
#include <system/interlocked.h>
#include <system/timer.h>
#include "queried_resources.h"
#include "resource_cook.h"
#include "resource_type_registry.h"

namespace resource_system {

struct resource_system::query_data
{
	resource_cook* cook;
	union
	{
		struct
		{
			cook_functor functor;
			u32 thread_index;
		};
		struct
		{
			user_query_callback callback;
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

		uptr const memory_size = queue_memory_size * m_thread_count + helper_queue_memory_size;

		pointer const memory = virtual_allocator( ).allocate( memory_size );

		pointer p = memory;

		for ( u32 i = 0; i < m_thread_count; ++i )
		{
			m_queues[i].create( p, queue_memory_size );
			p += queue_memory_size;
		}

		m_helper_queue.create( p, helper_queue_memory_size );
	}
	
	m_temp_allocator.create( );
	
	u32 const event_count = m_thread_count - engine_free_threads_first;
	for ( u32 i = 0; i < event_count; ++i )
		m_thread_events[i].create( );
}

void resource_system::stop( )
{
	m_keep_processing = false;

	u32 const event_count = m_thread_count - engine_free_threads_first;
	for ( u32 i = 0; i < event_count; ++i )
		m_thread_events[i].release( );
}

void resource_system::destroy( )
{
	u32 const event_count = m_thread_count - engine_free_threads_first;
	for ( u32 i = 0; i < event_count; ++i )
		m_thread_events[i].release( );

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

	if ( data->cook )
		data->functor( data->cook, resources );
	else
	{
		ASSERT( data->results_count != 0 );
		data->callback( resources, data->data, data->data_size );
	}

	uptr const data_size = data->size( );
	m_temp_allocator.deallocate( data, data_size );
}

void resource_system::busy_thread_job( u32 const in_thread_index, u64 const in_time_limit )
{
	ASSERT_CMP( in_thread_index, <, engine_free_threads_first );

	u32 const queue_index = in_thread_index;

	u64 current_tick;

	do
	{
		query_data* data;
		bool const pop_successful = m_queues[queue_index].pop( data );

		if ( !pop_successful )
			break;

		process_task( data );

		current_tick = sys::tick( );
	}
	while ( current_tick < in_time_limit );
}

void resource_system::free_thread_job( u32 const in_thread_index )
{
	ASSERT_CMP( in_thread_index, >=, engine_free_threads_first );
	ASSERT_CMP( in_thread_index, <, engine_helper_threads_first );

	u32 const queue_index = in_thread_index;
	u32 const event_index = in_thread_index - engine_free_threads_first;

	while ( m_keep_processing )
	{
		query_data* data;
		bool const pop_successful = m_queues[queue_index].pop( data );

		if ( pop_successful )
			process_task( data );
		else
			m_thread_events[event_index].enter( );
	}
}

void resource_system::helper_thread_job( u32 const in_thread_index )
{
	ASSERT_CMP( in_thread_index, >=, engine_helper_threads_first );
	ASSERT_CMP( in_thread_index, <, engine_helper_threads_first + m_helper_thread_count );

	u32 const queue_index = in_thread_index;
	u32 const event_index = in_thread_index - engine_free_threads_first;

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
			m_thread_events[event_index].enter( );
	}
}

void resource_system::query_resources(
	resource_cook* const* const in_cooks,
	u32 const in_cook_count,
	user_query_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size
)
{
	ASSERT( in_cooks != nullptr );
	ASSERT( in_cook_count != 0 );

	uptr const query_data_size = query_data::calculate_size( in_cook_count );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->cook = nullptr;
	data->callback = in_callback;
	data->data = in_callback_data;
	data->data_size = in_callback_data_size;
	data->results_count = in_cook_count;
	data->pending_count = in_cook_count;

	push_cook_queries( in_cooks, in_cook_count, data );
}

void resource_system::query_child_resources(
	resource_cook* const* const in_requested_cooks,
	u32 const in_requested_cook_count,
	resource_cook* const in_parent_cook,
	cook_functor const& in_functor,
	u32 const in_functor_thread_index
)
{
	ASSERT( in_requested_cooks != nullptr );
	ASSERT( in_requested_cook_count != 0 );
	ASSERT( in_parent_cook != nullptr );

	uptr const query_data_size = query_data::calculate_size( in_requested_cook_count );
	query_data* const data = m_temp_allocator.allocate( query_data_size );

	data->cook = in_parent_cook;
	data->functor = in_functor;
	data->thread_index = in_functor_thread_index;
	data->results_count = in_requested_cook_count;
	data->pending_count = in_requested_cook_count;

	push_cook_queries( in_requested_cooks, in_requested_cook_count, data );
}

void resource_system::push_cook_queries(
	resource_cook* const* const in_cooks,
	u32 const in_cook_count,
	query_data* const in_query_data
)
{
	ASSERT( in_cooks != nullptr );
	ASSERT( in_cook_count != 0 );
	ASSERT( in_query_data != nullptr );

	u32 const query_data_offset = (u32)( (pointer)in_query_data - m_temp_allocator.data( ) );

	for ( u32 i = 0; i < in_cook_count; ++i )
	{
		resource_cook* const cook = in_cooks[i];

		cook->parent_query( ) = query_data_offset;
		cook->result_index( ) = i;

		uptr const query_data_size = query_data::calculate_size( 0 );
		query_data* const data = m_temp_allocator.allocate( query_data_size );

		auto const& registry_data = g_resource_type_registry[cook->type( )];

		data->cook = cook;
		data->functor = registry_data.executor;
		data->thread_index = registry_data.executor_thread_index;
		data->results_count = 0;
		data->pending_count = 0;

		push_query_data( data );
	}
}

void resource_system::push_query_data( query_data* const in_query_data )
{
	u32 const thread_index = in_query_data->cook ? in_query_data->thread_index : m_thread_count;
	ASSERT_CMP( thread_index, <=, m_thread_count );

	if ( thread_index == m_thread_count )
		m_helper_queue.push( in_query_data );
	else
		m_queues[thread_index].push( in_query_data );
}

void resource_system::finish_cook( resource_cook* const in_cook, query_result const in_result )
{
	query_data* const data = m_temp_allocator.data( ) + in_cook->parent_query( );
	ASSERT( m_temp_allocator.contains_pointer( data ) );

	data->results[in_cook->result_index( )] = in_result;

	if ( interlocked_dec( data->pending_count ) == 0 )
		push_query_data( data );
}

resource_system g_resource_system;

} // namespace resource_system