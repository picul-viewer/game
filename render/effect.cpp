#include "effect.h"

#include "render_resources.h"

namespace render {

void technique::create( )
{
	m_depth_stencil_state	= nullptr;
	m_blend_state			= nullptr;
	m_rasterizer_state		= nullptr;
	
	m_vertex_shader			= nullptr;
	m_pixel_shader			= nullptr;
	m_geometry_shader		= nullptr;
	
	m_blend_factor			= math::float4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_sample_mask			= 0xFFFFFFFF;
	m_stencil_ref			= 0;
}

void technique::destroy( )
{
	if ( !m_vertex_shader->is_null( ) )
		g_resources.get_shader_pool( ).free_vertex	( m_vertex_shader );
	if ( !m_pixel_shader->is_null( ) )
		g_resources.get_shader_pool( ).free_pixel	( m_pixel_shader );
	if ( !m_geometry_shader->is_null( ) )
		g_resources.get_shader_pool( ).free_geometry( m_geometry_shader );
}

void technique::apply( ) const
{
	apply( m_stencil_ref, m_blend_factor, m_sample_mask );
}

void technique::apply( u8 in_stencil_ref,
					   math::float4 in_blend_factor,
					   u32 in_sample_mask ) const
{
	if ( m_depth_stencil_state )
		m_depth_stencil_state->bind( in_stencil_ref );
	if ( m_blend_state )
		m_blend_state->bind( in_blend_factor, in_sample_mask );
	if ( m_rasterizer_state )
		m_rasterizer_state->bind( );

	if ( m_vertex_shader )
		m_vertex_shader->bind( );
	if ( m_geometry_shader )
		m_geometry_shader->bind( );
	if ( m_pixel_shader )
		m_pixel_shader->bind( );
}
void technique::set_vertex_shader( pcstr in_filename, u32 in_configuration )
{
	m_vertex_shader = g_resources.get_shader_pool( ).create_vertex( in_filename, in_configuration );
}

void technique::set_pixel_shader( pcstr in_filename, u32 in_configuration )
{
	m_pixel_shader = g_resources.get_shader_pool( ).create_pixel( in_filename, in_configuration );
}

void technique::set_geometry_shader( pcstr in_filename, u32 in_configuration )
{
	m_geometry_shader = g_resources.get_shader_pool( ).create_geometry( in_filename, in_configuration );
}

void technique::set_depth_stencil_state( depth_stencil_state::cook const& in_cook )
{
	m_depth_stencil_state = g_resources.get_pipeline_pool( ).create_depth_stencil( in_cook );
}

void technique::set_blend_state( blend_state::cook const& in_cook )
{
	m_blend_state = g_resources.get_pipeline_pool( ).create_blend( in_cook );
}

void technique::set_rasterizer_state( rasterizer_state::cook const& in_cook )
{
	m_rasterizer_state = g_resources.get_pipeline_pool( ).create_rasterizer( in_cook );
}

} // namespace render