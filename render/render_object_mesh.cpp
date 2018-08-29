#include "render_object_mesh.h"

#include "resources.h"

namespace render {

void render_object_mesh::create( binary_config& in_config )
{
	pcstr render_model_path	= in_config.read_str( );
	m_render_model			= g_resources.get_render_model_mesh_pool( ).get( render_model_path );

	m_local_transform		= in_config.read<math::float4x3>( );
}

void render_object_mesh::destroy( )
{
	//
}

void render_object_mesh::update( math::float4x3 const& in_transform )
{
	m_transform		= math::sse::combine_transforms( m_local_transform, in_transform );

	m_aabb			= m_render_model->get_aabb( );
	m_aabb.modify	( m_transform );
}

void render_object_mesh::render( ) const
{
	m_render_model->render( );
}

} // namespace render