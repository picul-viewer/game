#include "stage_forward_default.h"
#include "resources.h"
#include "renderer.h"
#include "parameters.h"
#include "api.h"
#include "statistics.h"

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
	RENDER_PROFILE_EVENT					( stage_forward_default );

	RENDER_DEBUG_EVENT						( stage_forward_default );

	render_target_view& backbuffer			= g_resources.get_backbuffer( );
	depth_stencil_view& depth_buffer		= g_resources.get_depth_buffer( ).get_dsv( );

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX						= 0.0f;
	viewport.TopLeftY						= 0.0f;
	viewport.Width							= g_parameters.screen_resolution.x;
	viewport.Height							= g_parameters.screen_resolution.y;
	viewport.MinDepth						= 0.0f;
	viewport.MaxDepth						= 1.0f;
	g_api.get_context( )->RSSetViewports	( 1, &viewport );

	backbuffer.bind							( depth_buffer );

	m_forward_default_effect.apply( );

	constant_buffer& per_instance_buffer	= g_resources.get_default_constant_buffer( resources::default_constant_buffer_type_per_instance );

	for ( uptr i = 0; i < g_renderer.m_render_meshes.size( ); ++i )
	{
		RENDER_DEBUG_EVENT					( render_object_mesh );

		render_object_mesh const* object	= g_renderer.m_render_meshes[i];

		math::sse::matrix world				= math::sse::matrix( math::float4x4( object->get_transform( ) ) );

		math::sse::matrix const& view_proj	= g_renderer.m_render_camera.get_view_projection( );

		math::sse::matrix const& wvp		= math::sse::combine_transforms( world, view_proj );

		forward_default_per_instance_data	per_instance_data;
		per_instance_data.m_WVP				= math::float4x4( wvp );

		per_instance_buffer.update			( per_instance_data );

		object->render( );
	}

	render_target_view( ).bind( );
}

} // namespace render