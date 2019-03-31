#include "render_object_mesh.h"

#include "resources.h"
#include "render_model_mesh.h"

namespace render {

void render_object_mesh::create( lib::reader& in_reader )
{
	ASSERT( in_reader.is_valid( ) );

	bool provide_render_model_config	= in_reader.read<bool>( );

	if ( provide_render_model_config )
	{
		render_model_mesh* const model	= g_resources.get_render_model_mesh_pool( ).create_resource( );
		model->create					( in_reader );
		m_render_model					= model;
	}
	else
	{
		pcstr render_model_path			= in_reader.read_str( );
		m_render_model					= g_resources.get_render_model_mesh_pool( ).load_resource( render_model_path );
	}
	
	m_local_transform.loadu				( (float const*)in_reader.read_data( sizeof(math::sse::matrix3) ) );
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