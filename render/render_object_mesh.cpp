#include "render_object_mesh.h"

#include "resources.h"

namespace render {

void render_object_mesh::create( binary_config& in_config )
{
	ASSERT( in_config.is_valid( ) );

	bool provide_render_model_config	= in_config.read<bool>( );

	if ( provide_render_model_config )
	{
		m_render_model					= g_resources.get_render_model_mesh_pool( ).create_resource( );
		m_render_model->create			( in_config );
	}
	else
	{
		pcstr render_model_path			= in_config.read_str( );
		m_render_model					= g_resources.get_render_model_mesh_pool( ).load_resource( render_model_path );
	}
	
	m_local_transform.loadu				( (float const*)in_config.read_data( sizeof(math::sse::matrix3) ) );
}

void render_object_mesh::destroy( )
{
	g_resources.get_render_model_mesh_pool( ).free_resource( m_render_model );
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