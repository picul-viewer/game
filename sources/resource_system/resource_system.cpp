#include "resource_system.h"
#include <lib/allocator.h>
#include <math/math_common.h>
#include <system/interlocked.h>
#include "shared_resource_container_allocator.h"
#include <Windows.h>

namespace resource_system {

struct resource_system::task_data
{
	task_functor functor;
	pointer user_data;
	task_data* parent;
	u32 parent_index;
	u32 thread_index;
	u32 results_count;
	mt_u32 pending_count;

#pragma warning (push)
#pragma warning (disable:4200)
	pvoid results[0];
#pragma warning (pop)

	static uptr calculate_size( u32 const in_results_count );
	uptr size( ) const;
};

uptr resource_system::task_data::calculate_size( u32 const in_results_count )
{
	return sizeof(task_data) + sizeof(pvoid) * in_results_count;
}

uptr resource_system::task_data::size( ) const
{
	return calculate_size( results_count );
}


void resource_system::create( u32 const in_thread_count )
{
	_internal::s_shared_resource_container_allocator.create_shared_resource_containers( );

	m_thread_count = math::min( in_thread_count, (u32)engine_thread_count );

	{
		uptr const queue_size = 8 * 1024;
		uptr const helper_queue_size = 32 * 1024;

		uptr const queue_memory_size = queue_size * sizeof(task_data*);
		uptr const helper_queue_memory_size = helper_queue_size * sizeof(task_data*);

		uptr const memory_size = queue_memory_size * ( m_thread_count - ignore_thread_count ) + helper_queue_memory_size;

		pointer const memory = virtual_allocator( ).allocate( memory_size );

		pointer p = memory;

		for ( u32 i = 0; i < m_thread_count; ++i )
			if ( ( ( 1 << i ) & ignore_thread_mask ) == 0 )
			{
				m_queues[i].create( p, queue_size );
				p += queue_memory_size;
			}
			else
				m_queues[i].create( nullptr, 0 );

		m_helper_queue.create( p, helper_queue_size );
		p += helper_queue_memory_size;

		ASSERT( p == memory + memory_size );
	}

	m_temp_allocator.create( );

	for ( u32 i = 0; i < m_thread_count; ++i )
		m_events[i].create( false, false );
	m_events[engine_thread_count].create( false, false );

	for ( u32 i = 0; i < m_thread_count; ++i )
	{
		m_keep_process[i] = true;
		m_is_sleeping[i] = false;
	}

	for ( u32 i = 0; i < engine_thread_count; ++i )
		m_thread_local_data[i].current_task_data = nullptr;
}

bool resource_system::queues_empty( ) const
{
	bool result = true;

	for ( u32 i = 0; i < m_thread_count; ++i )
		result = result && m_queues[i].empty( );
	result = result && m_helper_queue.empty( );

	return result;
}

bool resource_system::all_asleep( ) const
{
	bool result = true;

	for ( u32 i = 1; i < m_thread_count; ++i )
		result = result && m_is_sleeping[i];

	return result;
}

void resource_system::stop( )
{
	ASSERT_CMP( utils::thread_index( ), ==, engine_thread_main );

	do
	{
		while ( process_task( engine_thread_main ) );
		Sleep( 1 );
	}
	while ( !queues_empty( ) );

	for ( u32 i = 0; i <= engine_thread_count; ++i )
		m_events[i].set( );
}

void resource_system::destroy( )
{
	for ( u32 i = 0; i < m_thread_count; ++i )
		m_events[i].destroy( );
	m_events[engine_thread_count].destroy( );

	m_temp_allocator.destroy( );

	pointer const queue_memory = m_queues[0].data( );

	for ( u32 i = 0; i < m_thread_count; ++i )
		m_queues[i].destroy( );
	m_helper_queue.destroy( );

	virtual_allocator( ).deallocate( queue_memory );

	_internal::s_shared_resource_container_allocator.destroy_shared_resource_containers( );
}

void resource_system::process_task( u32 const thread_index, task_data* const data )
{
	ASSERT( m_temp_allocator.contains_pointer( data ) );
	ASSERT_CMP( data->pending_count, ==, 0 );

	ASSERT( m_thread_local_data[thread_index].current_task_data == nullptr );
	m_thread_local_data[thread_index].current_task_data = data;

	data->functor( data->results, data->results_count, data->user_data );

	// If parent task collects children results,
	// then decrement counter when task result is set.
	if ( data->parent && ( data->parent->results_count == 0 ) )
	{
		if ( interlocked_dec( data->parent->pending_count ) == 0 )
			push_task( data->parent );
	}

	m_thread_local_data[thread_index].current_task_data = nullptr;

	uptr const data_size = data->size( );
	m_temp_allocator.deallocate( data, data_size );
}

bool resource_system::process_task( u32 const in_thread_index )
{
	task_data* data;
	bool const pop_successful = m_queues[in_thread_index].pop( data );

	if ( pop_successful )
		process_task( in_thread_index, data );

	return pop_successful;
}

bool resource_system::process_helper_task( )
{
	u32 const thread_index = utils::thread_index( );

	task_data* data;
	bool const pop_successful = m_helper_queue.pop( data );

	if ( pop_successful )
		process_task( thread_index, data );

	return pop_successful;
}

sys::system_event const& resource_system::queue_event( u32 const in_thread_index ) const
{
	return m_events[in_thread_index];
}

sys::system_event const& resource_system::helper_queue_event( ) const
{
	return m_events[engine_thread_count];
}

void resource_system::create_tasks(
	task_info const* const in_parent,
	task_info const* const in_children,
	u32 const in_children_count,
	bool in_collect_children_results
)
{
	ASSERT( in_children != nullptr );
	ASSERT( in_children_count != 0 );

	ASSERT( in_parent || !in_collect_children_results );

	task_data* parent_data = nullptr;

	if ( in_parent != nullptr )
	{
		u32 const children_count = in_collect_children_results ? in_children_count : 0;

		uptr const parent_data_size = task_data::calculate_size( children_count );
		parent_data = m_temp_allocator.allocate( parent_data_size );

		parent_data->functor = in_parent->functor;
		parent_data->user_data = in_parent->user_data;
		parent_data->parent = nullptr;
		parent_data->parent_index = -1;
		parent_data->thread_index = in_parent->thread_index;
		parent_data->results_count = children_count;
		parent_data->pending_count = in_children_count;
	}

	for ( u32 i = 0; i < in_children_count; ++i )
	{
		task_data* const data = m_temp_allocator.allocate( task_data::calculate_size( 0 ) );

		data->functor = in_children[i].functor;
		data->user_data = in_children[i].user_data;
		data->parent = parent_data;
		data->parent_index = in_collect_children_results ? i : -1;
		data->thread_index = in_children[i].thread_index;
		data->results_count = 0;
		data->pending_count = 0;

		push_task( data );
	}
}

void resource_system::create_subtasks(
	task_info const* const in_parent,
	task_info const* const in_children,
	u32 const in_children_count,
	bool in_collect_children_results
)
{
	ASSERT( in_children != nullptr );
	ASSERT( in_children_count != 0 );

	ASSERT( in_parent || !in_collect_children_results );

	u32 const thread_index = utils::thread_index( );
	ASSERT_CMP( thread_index, !=, engine_thread_count );
	task_data* const current_data = m_thread_local_data[thread_index].current_task_data;

	task_data* parent_data = current_data ? current_data->parent : nullptr;

	if ( in_parent )
	{
		u32 const children_count = in_collect_children_results ? in_children_count : 0;

		uptr const parent_data_size = task_data::calculate_size( children_count );
		parent_data = m_temp_allocator.allocate( parent_data_size );

		parent_data->functor = in_parent->functor;
		parent_data->user_data = in_parent->user_data;
		parent_data->parent = current_data ? current_data->parent : nullptr;
		parent_data->parent_index = current_data ? current_data->parent_index : -1;
		parent_data->thread_index = in_parent->thread_index;
		parent_data->results_count = children_count;
		parent_data->pending_count = in_children_count;
	}

	if ( current_data && current_data->parent && ( current_data->parent->results_count == 0 ) )
	{
		if ( in_parent )
			interlocked_inc( current_data->parent->pending_count );
		else
			interlocked_add( current_data->parent->pending_count, in_children_count );
	}

	for ( u32 i = 0; i < in_children_count; ++i )
	{
		task_data* const data = m_temp_allocator.allocate( task_data::calculate_size( 0 ) );

		data->functor = in_children[i].functor;
		data->user_data = in_children[i].user_data;
		data->parent = parent_data;
		data->parent_index = in_collect_children_results ? i : -1;
		data->thread_index = in_children[i].thread_index;
		data->results_count = 0;
		data->pending_count = 0;

		push_task( data );
	}
}

void resource_system::push_task( task_data* const in_task_data )
{
	u32 const thread_index = in_task_data->thread_index;
	ASSERT_CMP( thread_index, <=, engine_thread_count );

	if ( thread_index == engine_thread_count )
	{
		if ( m_helper_queue.push( in_task_data ) )
			m_events[engine_thread_count].set( );
	}
	else
	{
		ASSERT_CMP( thread_index, <, m_thread_count );
		ASSERT( ( ( 1 << thread_index ) & ignore_thread_mask ) == 0 );

		if ( m_queues[thread_index].push( in_task_data ) )
			m_events[thread_index].set( );
	}
}

custom_task_context resource_system::create_custom_tasks(
	u32 const in_count,
	task_info const& in_parent
)
{
	task_data* const parent_data = m_temp_allocator.allocate( task_data::calculate_size( 0 ) );

	parent_data->functor = in_parent.functor;
	parent_data->user_data = in_parent.user_data;
	parent_data->parent = nullptr;
	parent_data->parent_index = -1;
	parent_data->thread_index = in_parent.thread_index;
	parent_data->results_count = 0;
	parent_data->pending_count = in_count;

	return (custom_task_context)parent_data;
}

custom_task_context resource_system::create_custom_subtasks(
	u32 const in_count,
	task_info const* const in_parent
)
{
	u32 const thread_index = utils::thread_index( );
	ASSERT_CMP( thread_index, !=, engine_thread_count );
	task_data* const current_data = m_thread_local_data[thread_index].current_task_data;

	task_data* parent_data = current_data ? current_data->parent : nullptr;

	if ( in_parent )
	{
		parent_data = m_temp_allocator.allocate( task_data::calculate_size( 0 ) );

		parent_data->functor = in_parent->functor;
		parent_data->user_data = in_parent->user_data;
		parent_data->parent = current_data ? current_data->parent : nullptr;
		parent_data->parent_index = current_data ? current_data->parent_index : -1;
		parent_data->thread_index = in_parent->thread_index;
		parent_data->results_count = 0;
		parent_data->pending_count = in_count;
	}

	if ( current_data && current_data->parent && ( current_data->parent->results_count == 0 ) )
	{
		if ( in_parent )
			interlocked_inc( current_data->parent->pending_count );
		else
			interlocked_add( current_data->parent->pending_count, in_count );
	}

	return (custom_task_context)parent_data;
}

void resource_system::set_current_task_result(
	pvoid const in_result
)
{
	u32 const thread_index = utils::thread_index( );
	ASSERT_CMP( thread_index, !=, engine_thread_count );
	task_data* const current_data = m_thread_local_data[thread_index].current_task_data;

	ASSERT( current_data );

	task_data* const parent_data = current_data->parent;
	ASSERT( parent_data );

	u32 const result_index = current_data->parent_index;
	ASSERT( result_index != -1 );
	ASSERT_CMP( result_index, <, parent_data->results_count );

	parent_data->results[result_index] = in_result;

	if ( interlocked_dec( parent_data->pending_count ) == 0 )
		push_task( parent_data );
}

void resource_system::set_task_result(
	u32 const in_task_data_offset,
	u32 const in_result_index,
	pvoid const in_result
)
{
	task_data* const task_data = m_temp_allocator.data( ) + in_task_data_offset;
	ASSERT( m_temp_allocator.contains_pointer( task_data ) );

	ASSERT( in_result_index != -1 );
	ASSERT_CMP( in_result_index, <, task_data->results_count );

	task_data->results[in_result_index] = in_result;

	if ( interlocked_dec( task_data->pending_count ) == 0 )
		push_task( task_data );
}

void resource_system::finish_custom_task( custom_task_context const in_context )
{
	ASSERT( in_context != nullptr );

	task_data* const data = (task_data*)in_context;

	if ( interlocked_dec( data->pending_count ) == 0 )
		push_task( data );
}

void resource_system::get_current_result_data( u32& in_parent_data_offset, u32& in_result_index ) const
{
	u32 const thread_index = utils::thread_index( );
	ASSERT_CMP( thread_index, !=, engine_thread_count );
	task_data* const current_data = m_thread_local_data[thread_index].current_task_data;

	ASSERT( current_data );

	pointer const parent_data = current_data->parent;
	ASSERT( parent_data );
	ASSERT( current_data->parent_index != -1 );

	in_parent_data_offset = (u32)( parent_data - m_temp_allocator.data( ) );
	in_result_index = current_data->parent_index;
}

resource_system g_resource_system;

} // namespace resource_system