#include "render_model_mesh.h"

#include "render_resources.h"

namespace render {

void render_model_mesh::create( config& in_config )
{
	mem_align(16)
	math::float4 aabb_min, aabb_max;
	aabb_min					= in_config.read<math::float3>( );
	aabb_min					= in_config.read<math::float3>( );
	m_aabb.set_min_max			( aabb_min, aabb_max );

	pcstr mesh_path				= in_config.read<pcstr>( );
	m_mesh						= resources::get_mesh_pool( ).create_resource( mesh_path );

	pcstr diffuse_texture_path	= in_config.read<pcstr>( );
	m_diffuse					= resources::get_texture_pool( ).create_resource( diffuse_texture_path );
	pcstr specular_texture_path	= in_config.read<pcstr>( );
	m_specular					= resources::get_texture_pool( ).create_resource( specular_texture_path );
}

void render_model_mesh::destroy( )
{
	resources::get_mesh_pool( ).free_resource( m_mesh );

	resources::get_texture_pool( ).free_resource( m_diffuse );
	resources::get_texture_pool( ).free_resource( m_specular );
}

void render_model_mesh::render( ) const
{
	m_diffuse->bind_ps	( 0 );
	m_specular->bind_ps	( 1 );

	m_mesh->draw		( );
}

} // namespace render