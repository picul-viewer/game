#include "texture_cook.h"

#include <lib/allocator.h>
#include <lib/hash.h>
#include <lib/reader.h>
#include <lib/strings.h>
#include <resource_system/api.h>

#include "dds_loader.h"
#include "gpu_uploader.h"
#include "texture.h"
#include "resources.h"

namespace render {

texture_cook* texture_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = length + 1;

	texture_cook* const result = std_allocator( ).allocate( sizeof(texture_cook) + size );

	u32 const id = lib::hash32( in_path, length );
	result->init( id );

	result->m_length = length;
	strings::copy_n( result->m_path, in_path, size );

	return result;
}

void texture_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void texture_cook::create_resource( )
{
	check_resource_exists( &texture_cook::query_file );
}

void texture_cook::query_file( texture* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	raw_data_cook* const cook = raw_data_cook::create( m_path );

	create_child_resources( callback_task<&texture_cook::on_file_loaded>( engine_helper_thread_0 ), cook );
}

void texture_cook::on_file_loaded( queried_resources& in_queried )
{
	m_raw_data.reset( );
	m_raw_data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( m_raw_data != nullptr );

	lib::reader r( m_raw_data->data( ), m_raw_data->size( ) );

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;

	lib::buffer_array<gpu_upload_task> upload_tasks( m_upload_tasks, max_upload_tasks );

	dds_loader::load( r, m_result->m_texture, srv_desc, upload_tasks );

	m_result->m_texture_id = g_resources.create_texture( m_result->m_texture, srv_desc );

	custom_task_context const task_context = create_child_custom_tasks(
		(u32)upload_tasks.size( ),
		resource_system::user_callback_task<texture_cook, &texture_cook::on_gpu_upload_finished>( this )
	);

	for ( u32 i = 0; i < upload_tasks.size( ); ++i )
		upload_tasks[i].set_task_context( task_context );

	g_gpu_uploader.push_background_tasks( upload_tasks.data( ), (u32)upload_tasks.size( ) );
}

void texture_cook::on_gpu_upload_finished( )
{
	m_raw_data.reset( );

	finish( m_result );
}

} // namespace render