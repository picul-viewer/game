#include "render_model_mesh.h"

#include "resources.h"
#include <math/math_sse.h>

namespace render {

render_model_mesh::render_model_mesh( )
{ }

void render_model_mesh::create( config& in_config )
{
	math::sse::vector aabb_min, aabb_max;
	aabb_min.loadu				( (float const*)in_config.current( ) );
	in_config					+= sizeof( math::float3 );
	aabb_min.loadu				( (float const*)in_config.current( ) );
	in_config					+= sizeof( math::float3 );
	m_aabb.set_min_max			( aabb_min, aabb_max );

	pcstr mesh_path				= in_config.read<pcstr>( );
	m_mesh						= g_resources.get_mesh_pool( ).get( mesh_path );

	pcstr diffuse_texture_path	= in_config.read_str( );
	if ( diffuse_texture_path[0] != '\0' )
		m_diffuse				= g_resources.get_texture_pool( ).get( diffuse_texture_path );
	else
		m_diffuse				= nullptr;

	pcstr specular_texture_path	= in_config.read_str( );
	if ( specular_texture_path[0] != '\0' )
		m_specular				= g_resources.get_texture_pool( ).get( specular_texture_path );
	else
		m_specular				= nullptr;
}

void render_model_mesh::destroy( )
{
	//g_resources.get_mesh_pool( ).free_resource( m_mesh );

	//g_resources.get_texture_pool( ).free_resource( m_diffuse );
	//g_resources.get_texture_pool( ).free_resource( m_specular );
}

void render_model_mesh::render( ) const
{
	m_diffuse->bind_ps	( 0 );
	m_specular->bind_ps	( 1 );

	m_mesh->draw		( );
}

void render_model_mesh::load( render_model_mesh* out_resource, weak_const_string in_filename )
{
	sys::file f			( in_filename.c_str( ), sys::file::open_read );
	uptr const size		= f.size( );

	ASSERT				( size < 64 * Kb );
	pvoid const memory	= alloca( size );

	uptr read_count		= f.read( memory, size );
	ASSERT				( read_count == size );
	f.close				( );

	config cfg			( memory, size );
	out_resource->create( cfg );
}

} // namespace render