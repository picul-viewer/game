#include "api.h"
#include "resource_system.h"
#include "raw_data.h"

void resource_system::create( u32 const in_thread_count )
{
	g_resource_system.create( in_thread_count );
}

void resource_system::stop( )
{
	g_resource_system.stop( );
}

void resource_system::destroy( )
{
	g_resource_system.destroy( );
}

void resource_system::create_resources(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
)
{
	g_resource_system.create_tasks(
		in_callback.functor ? &in_callback : nullptr,
		in_tasks,
		in_task_count,
		true
	);
}

void resource_system::destroy_resources(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
)
{
	g_resource_system.create_subtasks(
		in_callback.functor ? &in_callback : nullptr,
		in_tasks,
		in_task_count,
		false
	);
}

void resource_system::busy_thread_job( sys::time const in_time_limit )
{
	g_resource_system.busy_thread_job( in_time_limit );
}

void resource_system::free_thread_job( )
{
	g_resource_system.free_thread_job( );
}

void resource_system::helper_thread_job( )
{
	g_resource_system.helper_thread_job( );
}

task_info resource_system::null_callback( )
{
	task_info info;
	info.functor = nullptr;
	info.user_data = nullptr;
	info.thread_index = engine_thread_count;
	return info;
}