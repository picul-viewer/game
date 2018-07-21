#include "stage_forward_default.h"
#include "resources.h"
#include "renderer.h"

namespace render {

struct forward_default_per_instance_data
{
	math::float4x4 m_WVP;
};

void stage_forward_default::create( )
{
	m_forward_default_effect.create( );

	depth_stencil_state::cook dss_cook;
	dss_cook.set_depth( true, true, D3D11_COMPARISON_LESS_EQUAL );
	dss_cook.set_stencil( false );
	m_forward_default_effect[0].set_depth_stencil_state	( dss_cook );

	blend_state::cook bs_cook;
	bs_cook.set_blend( false, false );
	bs_cook.set_blend_for_rt( 0, false );
	m_forward_default_effect[0].set_blend_state( bs_cook );
	
	rasterizer_state::cook rs_cook;
	rs_cook.set( D3D11_FILL_SOLID, D3D11_CULL_NONE );
	m_forward_default_effect[0].set_rasterizer_state( rs_cook );
	
	m_forward_default_effect[0].set_vertex_shader	( vertex_shader_forward_default );
	m_forward_default_effect[0].set_pixel_shader	( pixel_shader_forward_default );
}

void stage_forward_default::destroy( )
{
	m_forward_default_effect.destroy( );
}

void stage_forward_default::execute( )
{
	render_target_view& backbuffer			= g_resources.get_backbuffer( );
	render_target_tex2d& depth_buffer		= g_resources.get_depth_buffer( );

	backbuffer.bind							( depth_buffer.get_dsv( ) );

	m_forward_default_effect.apply( );

	camera& current_camera					= *g_renderer.m_camera;

	constant_buffer& per_instance_buffer	= g_resources.get_default_constant_buffer( resources::default_constant_buffer_type_per_instance );

	for ( uptr i = 0; i < g_renderer.m_render_meshes.size( ); ++i )
	{
		render_object_mesh const* object	= g_renderer.m_render_meshes[i];

		math::sse::matrix world_transform	= math::sse::matrix( object->get_transform( ) );

		math::sse::matrix view_projection	= current_camera.get_view_projection( );

		math::sse::matrix wvp				= math::sse::combine_transforms( view_projection, world_transform );

		forward_default_per_instance_data	per_instance_data;
		per_instance_data.m_WVP				= math::float4x4( wvp );

		per_instance_buffer.update			( per_instance_data );

		object->render( );
	}
}

} // namespace render