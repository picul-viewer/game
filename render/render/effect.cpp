#include "effect.h"

#include "render_resources.h"

namespace render {

void effect::apply( u8					in_stencil_ref,
					math::float4 const&	in_blend_factor,
					u32					in_sample_mask ) const
{
	m_depth_stencil_state->bind( in_stencil_ref );
	m_blend_state->bind( in_blend_factor, in_sample_mask );
	m_rasterizer_state->bind( );

	m_vertex_shader->bind( );
	m_geometry_shader->bind( );
	m_pixel_shader->bind( );
}

void effect::destroy( )
{
	if ( !m_vertex_shader->is_null( ) )
		resources::get_shader_pool( ).free_vertex	( m_vertex_shader );
	if ( !m_pixel_shader->is_null( ) )
		resources::get_shader_pool( ).free_pixel	( m_pixel_shader );
	if ( !m_geometry_shader->is_null( ) )
		resources::get_shader_pool( ).free_geometry	( m_geometry_shader );
}

void effect::set_vertex_shader( pcstr in_filename, u32 in_configuration )
{
	m_vertex_shader = resources::get_shader_pool( ).create_vertex( in_filename, in_configuration );
}

void effect::set_pixel_shader( pcstr in_filename, u32 in_configuration )
{
	m_pixel_shader = resources::get_shader_pool( ).create_pixel( in_filename, in_configuration );
}

void effect::set_geometry_shader( pcstr in_filename, u32 in_configuration )
{
	m_geometry_shader = resources::get_shader_pool( ).create_geometry( in_filename, in_configuration );
}

void effect::set_depth_stencil_state( depth_stencil_state::cook const& in_cook )
{
	m_depth_stencil_state = resources::get_pipeline_pool( ).create_depth_stencil( in_cook );
}

void effect::set_blend_state( blend_state::cook const& in_cook )
{
	m_blend_state = resources::get_pipeline_pool( ).create_blend( in_cook );
}

void effect::set_rasterizer_state( rasterizer_state::cook const& in_cook )
{
	m_rasterizer_state = resources::get_pipeline_pool( ).create_rasterizer( in_cook );
}

} // namespace render