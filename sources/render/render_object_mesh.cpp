#include "render_object_mesh.h"
#include <utils/resources_path.h>

#include "model_mesh.h"

namespace render {

void render_object_mesh::initialize( lib::reader& in_reader, lib::buffer_array<task_info>& in_queries )
{
	pcstr const model_path = in_reader.read_str( );
	model_mesh_cook* const model_cook = model_mesh_cook::create( utils::get_resource_path( model_path ).c_str( ) );

	task_info& model_query = in_queries.emplace_back( );
	model_cook->fill_task_info( model_query );
	
	m_local_transform = in_reader.read<math::float4x3>( );
}

void render_object_mesh::set_queried_resources( queried_resources& in_queried )
{
	m_model = in_queried.get_resource<model_mesh_handle>( );
}

void render_object_mesh::destroy( )
{
	m_model.release( );
}

void render_object_mesh::update( math::float4x3 const& in_transform )
{
	m_transform		= math::sse::combine_transforms( m_local_transform, in_transform );

	m_aabb			= model_mesh::from_handle( m_model )->get_aabb( );
	m_aabb.modify	( m_transform );
}

void render_object_mesh::render( ) const
{
	model_mesh::from_handle( m_model )->render( );
}

} // namespace render