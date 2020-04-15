#include "mesh_cook.h"

#include <lib/allocator.h>
#include <lib/hash.h>
#include <lib/reader.h>
#include <lib/strings.h>

#include "mesh.h"
#include "resources.h"
#include "gpu_structures.h"

namespace render {

mesh_cook* mesh_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = sizeof(mesh_cook) + length + 1;

	mesh_cook* const result = std_allocator( ).allocate( sizeof(mesh_cook) + size );

	u32 const id = lib::hash32( in_path, length );
	result->init( id );

	result->m_length = length;
	strings::copy_n( result->m_path, in_path, size );

	return result;
}

void mesh_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void mesh_cook::create_resource( )
{
	check_resource_exists( &mesh_cook::query_file );
}

void mesh_cook::query_file( mesh* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	raw_data_cook* const cook = raw_data_cook::create( m_path );

	create_child_resources( callback_task<&mesh_cook::on_file_loaded>( engine_helper_thread_0 ), cook );
}

void mesh_cook::on_file_loaded( queried_resources& in_queried )
{
	m_raw_data.reset( );
	m_raw_data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( m_raw_data != nullptr );

	lib::reader r( m_raw_data->data( ), m_raw_data->size( ) );

	u32 const index_count = r.read<u32>( );
	u32 const vertex_count = r.read<u32>( );

	m_result->m_index_count = index_count;
	m_result->m_vertex_count = vertex_count;

	u32 const index_buffer_offset = g_resources.create_mesh_index_buffer( index_count );
	u32 const vertex_buffer_offset = g_resources.create_mesh_vertex_buffer( vertex_count );

	m_result->m_index_buffer_offset = index_buffer_offset;
	m_result->m_vertex_buffer_offset = vertex_buffer_offset;

	u32 const indices_size = index_count * sizeof(u16);
	pvoid const indices = r.read_data( indices_size );

	u32 const vertices_size = vertex_count * sizeof(math::float3);
	pvoid const vertices = r.read_data( vertices_size );

	u32 const data_size = vertex_count * ( sizeof(gpu::vertex_data) );
	pvoid const data = r.read_data( data_size );

	custom_task_context const task_context = create_child_custom_tasks(
		upload_count,
		resource_system::user_callback_task<mesh_cook, &mesh_cook::on_gpu_upload_finished>( this )
	);

	lib::buffer_array<gpu_upload_task> upload_tasks;
	upload_tasks.create( m_upload_tasks, upload_count, upload_count );

	upload_tasks[0].set_source_data( indices, indices_size );
	upload_tasks[0].set_buffer_upload( g_resources.index_buffer( ), index_buffer_offset * sizeof(u16) );
	upload_tasks[0].set_task_context( task_context );

	upload_tasks[1].set_source_data( vertices, vertices_size );
	upload_tasks[1].set_buffer_upload( g_resources.vertex_buffer( ), vertex_buffer_offset * sizeof(math::float3) );
	upload_tasks[1].set_task_context( task_context );

	upload_tasks[2].set_source_data( data, data_size );
	upload_tasks[2].set_buffer_upload( g_resources.vertex_data_buffer( ), vertex_buffer_offset * ( sizeof(gpu::vertex_data) ) );
	upload_tasks[2].set_task_context( task_context );

	g_gpu_uploader.push_background_tasks( upload_tasks.data( ), upload_count );
}

void mesh_cook::on_gpu_upload_finished( )
{
	m_raw_data.reset( );

	finish( m_result );
}

} // namespace render