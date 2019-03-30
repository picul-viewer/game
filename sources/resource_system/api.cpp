#include "api.h"
#include "callback_allocator.h"
#include "resource_system.h"
#include "resource_type_registry.h"

void resource_system::create( u32 const in_worker_thread_count )
{
	g_callback_allocator.create( );
	g_resource_type_registry.create( );
	g_resource_system.create( in_worker_thread_count );
}

void resource_system::destroy( )
{
	g_resource_system.destroy( );
	g_callback_allocator.destroy( );
}

void resource_system::process_request( request_ptr& in_request, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size )
{
	query_callback_id const callback_id = g_callback_allocator.register_callback( in_callback, in_callback_data, in_callback_data_size );

	g_resource_system.process_request( in_request, callback_id );
}

void resource_system::process_request_from_file( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size )
{
	query_callback_id const callback_id = g_callback_allocator.register_callback( in_callback, in_callback_data, in_callback_data_size );

	g_resource_system.process_request_from_file( in_file_path, callback_id );
}

void resource_system::query_file( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size )
{
	query_callback_id const callback_id = g_callback_allocator.register_callback( in_callback, in_callback_data, in_callback_data_size );

	g_resource_system.query_file( in_file_path, callback_id );
}

void resource_system::query_request( pcstr const in_file_path, query_callback const& in_callback, pointer const in_callback_data, uptr const in_callback_data_size )
{
	query_callback_id const callback_id = g_callback_allocator.register_callback( in_callback, in_callback_data, in_callback_data_size );

	g_resource_system.query_request( in_file_path, callback_id );
}