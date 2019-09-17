#include "api.h"
#include "resource_system.h"

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

void resource_system::process_busy( sys::time const in_time_limit )
{
	g_resource_system.process_busy( in_time_limit );
}

void resource_system::process_free( )
{
	g_resource_system.process_free( );
}

void resource_system::process_helper( )
{
	g_resource_system.process_helper( );
}

void resource_system::break_thread( u32 const in_thread_index )
{
	g_resource_system.break_thread( in_thread_index );
}

task_info resource_system::null_callback( )
{
	task_info info;
	info.functor = nullptr;
	info.user_data = nullptr;
	info.thread_index = engine_thread_count;
	return info;
}