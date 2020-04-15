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

void resource_system::execute_tasks(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
)
{
	g_resource_system.create_tasks(
		in_callback.functor ? &in_callback : nullptr,
		in_tasks,
		in_task_count,
		false
	);
}

void resource_system::finish_cusom_task( custom_task_context const in_context )
{
	g_resource_system.finish_custom_task( in_context );
}

custom_task_context resource_system::create_custom_tasks(
	u32 const in_task_count,
	task_info const& in_callback
)
{
	return g_resource_system.create_custom_tasks(
		in_task_count,
		in_callback
	);
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

bool resource_system::process_task( u32 const in_thread_index )
{
	return g_resource_system.process_task( in_thread_index );
}

bool resource_system::process_helper_task( )
{
	return g_resource_system.process_helper_task( );
}

sys::system_event const& resource_system::queue_event( u32 const in_thread_index )
{
	return g_resource_system.queue_event( in_thread_index );
}

sys::system_event const& resource_system::helper_queue_event( )
{
	return g_resource_system.helper_queue_event( );
}

task_info resource_system::null_callback( )
{
	task_info info;
	info.functor = nullptr;
	info.user_data = nullptr;
	info.thread_index = engine_thread_count;
	return info;
}