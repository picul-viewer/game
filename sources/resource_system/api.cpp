#include "api.h"
#include "resource_system.h"
#include "raw_data.h"

void resource_system::create( u32 const in_thread_count )
{
	g_resource_system.create( in_thread_count );

	register_resource_type<raw_data>( );
}

void resource_system::stop( )
{
	g_resource_system.stop( );
}

void resource_system::destroy( )
{
	g_resource_system.destroy( );
}

void resource_system::query_resources(
	resource_cook* const* const in_cooks,
	u32 const in_cook_count,
	user_query_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size
)
{
	g_resource_system.query_resources(
		in_cooks,
		in_cook_count,
		in_callback,
		in_callback_data,
		in_callback_data_size
	);
}

void resource_system::busy_thread_job( u32 const in_thread_index, sys::time const in_time_limit )
{
	g_resource_system.busy_thread_job( in_thread_index, in_time_limit );
}

void resource_system::free_thread_job( u32 const in_helper_thread_index )
{
	g_resource_system.free_thread_job( in_helper_thread_index );
}

void resource_system::helper_thread_job( u32 const in_helper_thread_index )
{
	g_resource_system.helper_thread_job( in_helper_thread_index );
}