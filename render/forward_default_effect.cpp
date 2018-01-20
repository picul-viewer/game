#include "forward_default_effect.h"
#include "render_resources.h"
#include "constant_buffers.h"

namespace render {

void forward_default_effect::create( config in_config )
{
	m_exist_texture					= false;
	m_exist_value					= false;

	u8 id;

	// Diffuse
	id								= in_config.read<u8>( );
	ASSERT							( ( id == config_value::diffuse_texture ) || ( config_value::diffuse_value ) );
	bool use_diffuse_texture		= id == config_value::diffuse_texture;
	if ( use_diffuse_texture )
	{
		pcstr texture_path			= in_config.read_str( );
		m_diffuse_texture			= resources::get_texture_pool( ).create_resource( texture_path );
		m_exist_texture				= true;
	}
	else
	{
		m_constants.m_diffuse		= in_config.read<math::float3>( );
		m_exist_value				= true;
	}

	// Specular
	id								= in_config.read<u8>( );
	ASSERT							( ( id == config_value::specular_texture ) || ( config_value::specular_value ) );
	bool use_specular_texture		= id == config_value::specular_texture;
	if ( use_specular_texture )
	{
		pcstr texture_path			= in_config.read_str( );
		m_specular_texture			= resources::get_texture_pool( ).create_resource( texture_path );
		m_exist_texture				= true;
	}
	else
	{
		m_constants.m_specular		= in_config.read<float>( );
		m_exist_value				= true;
	}
	
	// Smoothness
	id								= in_config.read<u8>( );
	ASSERT							( ( id == config_value::smoothness_texture ) || ( config_value::smoothness_value ) );
	bool use_smoothness_texture		= id == config_value::smoothness_texture;
	if ( use_smoothness_texture )
	{
		pcstr texture_path			= in_config.read_str( );
		m_smoothness_texture		= resources::get_texture_pool( ).create_resource( texture_path );
		m_exist_texture				= true;
	}
	else
	{
		m_constants.m_smoothness	= in_config.read<float>( );
		m_exist_value				= true;
	}

	// Metalness
	id								= in_config.read<u8>( );
	ASSERT							( ( id == config_value::metalness_texture ) || ( config_value::metalness_value ) );
	bool use_metalness_texture		= id == config_value::metalness_texture;
	if ( use_metalness_texture )
	{
		pcstr texture_path			= in_config.read_str( );
		m_metalness_texture			= resources::get_texture_pool( ).create_resource( texture_path );
		m_exist_texture				= true;
	}
	else
	{
		m_constants.m_metalness		= in_config.read<float>( );
		m_exist_value				= true;
	}
	
	{
		depth_stencil_state::cook	cook;
		cook.set_depth				( true, true, D3D11_COMPARISON_LESS );
		cook.set_stencil			( false );
		set_depth_stencil_state		( cook );
	}
	
	{
		blend_state::cook			cook;
		cook.set_blend				( );
		cook.set_blend_for_rt		( 0, false );
		set_blend_state				( cook );
	}

	{
		rasterizer_state::cook		cook;
		cook.set					( D3D11_FILL_SOLID, D3D11_CULL_BACK );
		set_rasterizer_state		( cook );
	}

	{
		shader_configuration		vconf;
		vconf.set_vertex_type		( vertex_type_static_mesh );
		set_vertex_shader			( "forward_default.vs", vconf );
	}
	
	{
		shader_configuration		pconf;
		pconf.set					( pixel_shader_macro_use_diffuse_texture, use_diffuse_texture );
		pconf.set					( pixel_shader_macro_use_specular_texture, use_specular_texture );
		pconf.set					( pixel_shader_macro_use_smoothness_texture, use_smoothness_texture );
		pconf.set					( pixel_shader_macro_use_metalness_texture, use_metalness_texture );
		set_pixel_shader			( "forward_default.ps", pconf );
	}
}

void forward_default_effect::apply( u8					in_stencil_ref,
									math::float4 const&	in_blend_factor,
									u32					in_sample_mask ) const
{
	if ( m_exist_texture )
	{
		shader_resource_views views;
		views.set_shader_resource( 0, *m_diffuse_texture );
		views.set_shader_resource( 1, *m_specular_texture );
		views.set_shader_resource( 2, *m_smoothness_texture );
		views.set_shader_resource( 3, *m_metalness_texture );

		views.bind_ps( );
	}
	
	if ( m_exist_value )
	{
		constant_buffers::get_miscellaneous_constant_buffer( ).update( &m_constants, 0, sizeof(miscellaneous_constants) );
	}

	effect::apply( in_stencil_ref, in_blend_factor, in_sample_mask );
}

void forward_default_effect::destroy( )
{
	if ( m_diffuse_texture->get_view( ) )
		resources::get_texture_pool( ).free_resource( m_diffuse_texture );

	if ( m_specular_texture->get_view( ) )
		resources::get_texture_pool( ).free_resource( m_specular_texture );

	if ( m_smoothness_texture->get_view( ) )
		resources::get_texture_pool( ).free_resource( m_smoothness_texture );

	if ( m_metalness_texture->get_view( ) )
		resources::get_texture_pool( ).free_resource( m_metalness_texture );

	effect::destroy( );
}

} // namespace redner