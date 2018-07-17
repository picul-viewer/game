#include "effect.h"

#include "resources.h"

namespace render {

void technique::create( )
{
	m_blend_factor			= math::float4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_sample_mask			= 0xFFFFFFFF;
	m_stencil_ref			= 0;
}

void technique::destroy( )
{
	////
}

void technique::apply( ) const
{
	apply( m_stencil_ref, m_blend_factor, m_sample_mask );
}

void technique::apply( u8 in_stencil_ref,
					   math::float4 in_blend_factor,
					   u32 in_sample_mask ) const
{
	m_depth_stencil_state.bind( in_stencil_ref );
	m_blend_state.bind( in_blend_factor, in_sample_mask );
	m_rasterizer_state.bind( );

	m_vertex_shader.bind( );
	m_geometry_shader.bind( );
	m_pixel_shader.bind( );
}
void technique::set_vertex_shader( vertex_shader_type in_shader_type )
{
	m_vertex_shader = g_resources.get_vertex_shader_container( )[in_shader_type];
}

void technique::set_pixel_shader( pixel_shader_type in_shader_type )
{
	m_pixel_shader = g_resources.get_pixel_shader_container( )[in_shader_type];
}

void technique::set_geometry_shader( geometry_shader_type in_shader_type )
{
	m_geometry_shader = g_resources.get_geometry_shader_container( )[in_shader_type];
}

void technique::set_depth_stencil_state( depth_stencil_state::cook const& in_cook )
{
	m_depth_stencil_state = g_resources.get_pipeline_state_pool( ).create_depth_stencil( in_cook );
}

void technique::set_blend_state( blend_state::cook const& in_cook )
{
	m_blend_state = g_resources.get_pipeline_state_pool( ).create_blend( in_cook );
}

void technique::set_rasterizer_state( rasterizer_state::cook const& in_cook )
{
	m_rasterizer_state = g_resources.get_pipeline_state_pool( ).create_rasterizer( in_cook );
}

} // namespace render