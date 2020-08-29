#include "render_object_mesh.h"
#include <resources/resources_path.h>

#include "gpu_structures.h"
#include "model_mesh.h"
#include "resources.h"

namespace render {

void render_object_mesh::create( lib::reader& in_reader, lib::buffer_array<task_info>& in_queries )
{
	ASSERT( aligned( this, 16 ) );

	pcstr const model_path = in_reader.read_str( );
	model_mesh_cook* const model_cook = create_cook<model_mesh_cook>( get_resource_path( model_path ).c_str( ) );

	task_info& model_query = in_queries.emplace_back( );
	model_cook->fill_task_info( model_query );
	
	m_local_transform = in_reader.read<math::float4x3>( );
}

void render_object_mesh::on_resources_ready( queried_resources& in_queried, lib::buffer_array<gpu_upload_task>& in_tasks, linear_allocator& in_allocator, bool const in_is_static )
{
	m_model = in_queried.get_resource<model_mesh_handle>( );
	model_mesh* model = model_mesh::from_handle( m_model );

	if ( in_is_static )
	{
		m_transform_handle = g_resources.transforms( ).allocate_static( );
		g_resources.transforms( ).get_init_tasks( m_transform_handle, m_local_transform, in_tasks );
	}
	else
	{
		m_transform_handle = g_resources.transforms( ).allocate_dynamic( );
		update( math::float4x3::identity( ) );
	}

	m_object_handle = g_resources.create_mesh_object( );

	gpu::mesh_object* data = in_allocator.allocate( sizeof(gpu::mesh_object) );
	model->fill_gpu_data( *data );
	data->transform_index = m_transform_handle;

	gpu_upload_task& task = in_tasks.emplace_back( );
	task.set_source_data( data, sizeof(gpu::mesh_object) );
	task.set_buffer_upload( g_resources.mesh_object_buffer( ), sizeof(gpu::mesh_object) * m_object_handle );
}

void render_object_mesh::destroy( )
{
	g_resources.transforms( ).deallocate( m_transform_handle );
	g_resources.destroy_mesh_object( m_object_handle );
	m_model.release( );
}

void render_object_mesh::update( math::float4x3 const& in_transform )
{
	math::float4x3 const& transform	= math::sse::combine_transforms( m_local_transform, in_transform );

	m_aabb							= model_mesh::from_handle( m_model )->get_aabb( );
	m_aabb.modify					( transform );
	
	g_resources.transforms( ).update( m_transform_handle, transform );
}

} // namespace render