#include "resource_system.h"
#include <lib/allocator.h>
#include <lib/strings.h>
#include <math/math_common.h>
#include <system/file.h>
#include "callback_allocator.h"
#include "file_ptr.h"
#include "queried_resources.h"
#include "query_data.h"
#include "resource_type_registry.h"
#include "request_builder.h"

namespace resource_system {

void resource_system::create( u32 const in_worker_thread_count )
{
	{
		uptr const fs_queue_size = 64 * Kb;
		uptr const manager_queue_size = 64 * Kb;
		uptr const worker_queue_size = 128 * Kb;

		uptr const size = fs_queue_size + manager_queue_size + worker_queue_size;

		pointer const data = virtual_allocator( ).allocate( size );

		pointer const fs_queue_data = data;
		pointer const manager_queue_data = fs_queue_data + fs_queue_size;
		pointer const worker_queue_data = manager_queue_data + manager_queue_size;

		m_fs_queue.create		( fs_queue_data, fs_queue_size );
		m_manager_queue.create	( manager_queue_data, manager_queue_size );
		m_worker_queue.create	( worker_queue_data, worker_queue_size );
	}
	
	
	m_fs_thread.create( &thread::method_helper<resource_system, &resource_system::fs_thread>, 512 * Kb, this );

	m_manager_thread.create( &thread::method_helper<resource_system, &resource_system::manager_thread>, 512 * Kb, this );

	ASSERT_CMP( in_worker_thread_count, <=, max_worker_thread_count );

	m_worker_count = in_worker_thread_count;
	for ( u32 i = 0; i < in_worker_thread_count; ++i )
	{
		m_worker_threads[i].create( &thread::method_helper<resource_system, &resource_system::worker_thread>, 1 * Mb, this );
		m_worker_threads[i].advise_processor_index( i );
	}
}

void resource_system::destroy( )
{
	m_worker_threads->destroy( time::infinite, m_worker_count );
	m_manager_thread.destroy( time::infinite );
	m_fs_thread.destroy( time::infinite );

	m_fs_queue.destroy( );
	m_manager_queue.destroy( );
	m_worker_queue.destroy( );

	virtual_allocator( ).deallocate( m_fs_queue.data( ) );
}

void resource_system::fs_thread( )
{
	while ( true )
	{
		query_data* data;
		m_fs_queue.pop( data );
		
		ASSERT( data->is_fs_query( ) );

		u32 const dependent_queries_count = data->dependent_queries_count;
		u32* const dependent_queries = data->dependent_queries( );

		ASSERT( strings::length( data->cook_data( ) ) > 0 );

		query_result const result =
			( data->type == resource_type_request ) ?
			request_ptr::load( data->cook_data( ), dependent_queries_count ) :
			file_ptr::load( data->cook_data( ), dependent_queries_count );

		query_result* const result_slot = (pointer)data + data->index;
		*result_slot = result;
		
		for ( u32 *i = dependent_queries, *e = dependent_queries + dependent_queries_count; i != e; ++i )
		{
			query_data* const dependent_query_data = (pointer)data + *i;

			if ( interlocked_dec( dependent_query_data->waiting_factor ) == 0 )
			{
				push_query( dependent_query_data );
			}
		}
	}
}

bool resource_system::manage_resource( query_data* const in_data )
{
	auto const manager_functor = g_resource_type_registry[in_data->type].m_manager;

	ASSERT( manager_functor != nullptr );

	resource_instance_id instance_id = 0;
	bool const resource_exists = manager_functor( in_data->cook_data( ), instance_id );

	query_result* const result_slot = (pointer)in_data + in_data->index;
	*result_slot = instance_id;

	if ( resource_exists )
		return true;

	u32 const requested_queries_count = in_data->requested_queries_count;
	u32 const* const requested_queries_offsets = in_data->requested_queries( );

	in_data->waiting_factor = (u32)requested_queries_count;
	u32 last_waiting_factor = (u32)requested_queries_count;

	for ( u32 i = 0; i < requested_queries_count; ++i )
	{
		query_data* const requested_data = (query_data*)( (pointer)in_data + requested_queries_offsets[i] );

		bool const ready = manage_resource( requested_data );

		if ( ready )
			last_waiting_factor = interlocked_dec( in_data->waiting_factor );
	}

	if ( last_waiting_factor == 0 )
		push_query( in_data );

	return false;
}

void resource_system::manager_thread( )
{
	while ( true )
	{
		query_data* data;
		m_manager_queue.pop( data );

		ASSERT( data->type == finish_request_type );

		bool const resource_exists = manage_resource( data );

		if ( resource_exists )
		{
			static uptr const max_requested_queries_count = 1024;
			query_result requested_queries[max_requested_queries_count];

			u32 const requested_queries_count = data->requested_queries_count;
			u32 const* const requested_queries_offsets = data->requested_queries( );

			ASSERT_CMP( requested_queries_count, <, max_requested_queries_count );
			for ( u32 i = 0; i < requested_queries_count; ++i )
			{
				query_data const* const query = (pointer)data + requested_queries_offsets[i];
				requested_queries[i] = *(query_result*)( (pointer)query + query->index );
			}

			query_callback_id& callback_id = data->callback_id( );
			if ( callback_id != 0xFFFFFFFF )
				g_callback_allocator.invoke_callback( callback_id, queried_resources( requested_queries, requested_queries_count ) );

			request_header* const header = (pointer)data - data->index;
			request_ptr::release_by_pointer( header );
		}
	}
}

void resource_system::worker_thread( )
{
	static uptr const max_requested_queries_count = 1024;
	query_result requested_queries[max_requested_queries_count];

	while ( true )
	{
		query_data* data;
		m_worker_queue.pop( data );

		ASSERT_CMP( data->waiting_factor, ==, 0 );

		u32 const requested_queries_count = data->requested_queries_count;
		u32 const* const requested_queries_offsets = data->requested_queries( );

		ASSERT_CMP( requested_queries_count, <, max_requested_queries_count );
		for ( u32 i = 0; i < requested_queries_count; ++i )
		{
			query_data const* const query = (pointer)data + requested_queries_offsets[i];
			requested_queries[i] = *(query_result*)( (pointer)query + query->index );
		}

		if ( data->type != finish_request_type )
		{
			query_result const result = g_resource_type_registry[data->type].m_creator( data->cook_data( ), queried_resources( requested_queries, requested_queries_count ) );
		
			query_result* const result_slot = (pointer)data + data->index;
			*result_slot = result;
		
			u32 const dependent_queries_count = data->dependent_queries_count;
			u32* const dependent_queries = data->dependent_queries( );

			for ( u32 *i = dependent_queries, *e = dependent_queries + dependent_queries_count; i != e; ++i )
			{
				query_data* const dependent_query_data = (pointer)data + *i;

				if ( interlocked_dec( dependent_query_data->waiting_factor ) == 0 )
				{
					push_query( dependent_query_data );
				}
			}
		}
		else
		{
			query_callback_id& callback_id = data->callback_id( );
			if ( callback_id != 0xFFFFFFFF )
				g_callback_allocator.invoke_callback( callback_id, queried_resources( requested_queries, requested_queries_count ) );

			request_header* const header = (pointer)data - data->index;
			request_ptr::release_by_pointer( header );
		}
	}
}

void resource_system::push_query( query_data* const in_query )
{
	if ( in_query->is_fs_query( ) )
		m_fs_queue.push( in_query );
	else
		m_worker_queue.push( in_query );
}

void resource_system::manage_query( query_data* const in_query )
{
	ASSERT( in_query->type == finish_request_type );
	m_manager_queue.push( in_query );
}

void resource_system::process_request( request_ptr& in_request, query_callback_id const in_callback_id )
{
	in_request.submit( in_callback_id );
}

static void submit_request_func( queried_resources& in_queried, pointer const in_data )
{
	request_ptr request = std::move( in_queried.get_request( ) );
	query_callback_id const callback_id = *(query_callback_id*)in_data;
	g_resource_system.process_request( request, callback_id );
}

void resource_system::process_request_from_file( pcstr const in_file_path, query_callback_id const in_callback_id )
{
	request_builder builder;

	uptr const size = strings::length( in_file_path ) + 1;

	request_cook_data* const cook_data = stack_allocate( size );
	strings::copy_n( cook_data->file_path, in_file_path, size );

	query<request_cook_data>& query = builder.create_query( cook_data );
	builder.query_resource( query );

	request_ptr request = std::move( builder.compile( ) );

	query_callback_id const callback_id = g_callback_allocator.register_callback( submit_request_func, &in_callback_id, sizeof(query_callback_id) );

	process_request( request, callback_id );
}

void resource_system::query_file( pcstr const in_file_path, query_callback_id const in_callback_id )
{
	request_builder builder;

	uptr const size = strings::length( in_file_path ) + 1;

	file_cook_data* const cook_data = stack_allocate( size );
	strings::copy_n( cook_data->file_path, in_file_path, size );

	query<file_cook_data>& query = builder.create_query( cook_data );
	builder.query_resource( query );

	request_ptr request = std::move( builder.compile( ) );

	process_request( request, in_callback_id );
}

void resource_system::query_request( pcstr const in_file_path, query_callback_id const in_callback_id )
{
	request_builder builder;

	uptr const size = strings::length( in_file_path ) + 1;

	request_cook_data* const cook_data = stack_allocate( size );
	strings::copy_n( cook_data->file_path, in_file_path, size );

	query<request_cook_data>& query = builder.create_query( cook_data );
	builder.query_resource( query );

	request_ptr request = std::move( builder.compile( ) );

	process_request( request, in_callback_id );
}

resource_system g_resource_system;

} // namespace resource_system