#include "render_model_mesh.h"

#include "resources.h"
#include <math/math_sse.h>

namespace render {

void render_model_mesh::create( binary_config& in_config )
{
	ASSERT( in_config.is_valid( ) );

	// Read AABB.
	math::sse::vector aabb_min, aabb_max;
	aabb_min.loadu				( (float const*)in_config.read_data( sizeof(math::float3) ) );
	aabb_max.loadu				( (float const*)in_config.read_data( sizeof(math::float3) ) );
	m_aabb.set_min_max			( aabb_min, aabb_max );

	// Read mesh.
	u8 mesh_source_type			= in_config.read<u8>( );
	switch ( mesh_source_type )
	{
		case 255:
		{
			pcstr path			= in_config.read_str( );
			m_mesh				= g_resources.get_mesh_pool( ).load_resource( path );
			break;
		}
		case 254:
		{
			pcstr path			= in_config.read<pcstr>( );
			m_mesh				= g_resources.get_mesh_pool( ).load_resource( path );
			break;
		}
		case 253:
		{
			m_mesh				= g_resources.get_mesh_pool( ).create_resource( );
			m_mesh->create		( in_config );
			break;
		}
		default:
		{
			m_mesh				= &g_resources.get_default_mesh( mesh_source_type );
			break;
		}
	}
	
	// Read diffuse texture.
	u8 diffuse_source_type		= in_config.read<u8>( );
	switch ( diffuse_source_type )
	{
		case 0:
		{
			pcstr path			= in_config.read_str( );
			m_diffuse			= g_resources.get_texture_pool( ).load_resource( path );
			break;
		}
		case 1:
		{
			pcstr path			= in_config.read<pcstr>( );
			m_diffuse			= g_resources.get_texture_pool( ).load_resource( path );
			break;
		}
		case 2:
		{
			m_diffuse			= g_resources.get_texture_pool( ).create_resource( );
			m_diffuse->create	( in_config );
			break;
		}
		default:
			UNREACHABLE_CODE
	}

	// Read specular texture.
	u8 specular_source_type		= in_config.read<u8>( );
	switch ( specular_source_type )
	{
		case 0:
		{
			pcstr path			= in_config.read_str( );
			m_specular			= g_resources.get_texture_pool( ).load_resource( path );
			break;
		}
		case 1:
		{
			pcstr path			= in_config.read<pcstr>( );
			m_specular			= g_resources.get_texture_pool( ).load_resource( path );
			break;
		}
		case 2:
		{
			m_specular			= g_resources.get_texture_pool( ).create_resource( );
			m_specular->create	( in_config );
			break;
		}
		default:
			UNREACHABLE_CODE
	}

	// Initialize reference counter.
	m_ref_counter				= 1;
}

u32 render_model_mesh::add_ref( )
{
	return ++m_ref_counter;
}

u32 render_model_mesh::release( )
{
	u32 const ref_count = --m_ref_counter;

	if ( ref_count == 0 )
	{
		g_resources.get_mesh_pool( ).free_resource( m_mesh );

		g_resources.get_texture_pool( ).free_resource( m_diffuse );
		g_resources.get_texture_pool( ).free_resource( m_specular );
	}

	return ref_count;
}

void render_model_mesh::set_registry_pointer( pointer in_pointer )
{
	m_registry_pointer = in_pointer;
}

pointer render_model_mesh::get_registry_pointer( ) const
{
	return m_registry_pointer;
}

void render_model_mesh::render( ) const
{
	m_diffuse->bind_ps			( 0 );
	m_specular->bind_ps			( 1 );

	m_mesh->draw				( );
}

} // namespace render