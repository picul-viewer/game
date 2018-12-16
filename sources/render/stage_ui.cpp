#include "stage_ui.h"
#include "resources.h"
#include "renderer.h"
#include "parameters.h"
#include "api.h"
#include "ui_batch.h"
#include "statistics.h"

namespace render {

void stage_ui::create( )
{
	m_ui_default_effect.create( );

	depth_stencil_state::cook dss_cook;
	dss_cook.set_depth( false );
	dss_cook.set_stencil( false );
	m_ui_default_effect[0].set_depth_stencil_state( dss_cook );

	blend_state::cook bs_cook;
	bs_cook.set_blend( false, false );
	bs_cook.set_blend_for_rt(
		0, true,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD );
	m_ui_default_effect[0].set_blend_state( bs_cook );
	
	rasterizer_state::cook rs_cook;
	rs_cook.set( D3D11_FILL_SOLID, D3D11_CULL_NONE );
	m_ui_default_effect[0].set_rasterizer_state( rs_cook );
	
	m_ui_default_effect[0].set_vertex_shader( vertex_shader_ui_default );
	m_ui_default_effect[0].set_pixel_shader	( pixel_shader_ui_default );
}

void stage_ui::destroy( )
{
	m_ui_default_effect.destroy( );
}

void stage_ui::execute( )
{
	RENDER_PROFILE_EVENT					( stage_ui );
	
	RENDER_DEBUG_EVENT						( stage_ui );

	ui_batch& batch							= g_resources.get_ui_batch( );
	auto const& batch_data					= batch.get_batch_data( );
	if ( batch_data.size( ) == 0 )
		return;

	render_target_view& backbuffer			= g_resources.get_backbuffer( );

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX						= 0.0f;
	viewport.TopLeftY						= 0.0f;
	viewport.Width							= g_parameters.screen_resolution.x;
	viewport.Height							= g_parameters.screen_resolution.y;
	viewport.MinDepth						= 0.0f;
	viewport.MaxDepth						= 1.0f;
	g_api.get_context( )->RSSetViewports	( 1, &viewport );

	backbuffer.bind							( );

	m_ui_default_effect.apply				( );
	
	pcvoid const buffer						= batch.get_buffer( );
	u32 const buffer_size					= batch.get_buffer_size( );

	vertex_buffer vb						= g_resources.get_ui_vertex_buffer( );
	index_buffer ib							= g_resources.get_ui_index_buffer( );

	vb.update_dynamic						( buffer, buffer_size );

	vb.bind									( 0, 0 );
	ib.bind									( 0 );

	g_api.get_context( )->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	for ( uptr i = 0; i < batch_data.size( ); ++i )
	{
		ui_batch::batch_data const& data	= batch_data[i];

		texture_handle const current_tex	= data.texture;
		if ( current_tex != texture_handle::invalid )
			current_tex->bind_ps			( 0 );
		else
		{
			texture null_tex;
			null_tex.set					( nullptr );
			null_tex.bind_ps				( 0 );
		}

		g_api.get_context( )->DrawIndexed	( data.vertex_count / 4 * 6, 0, data.vertex_start_index );
	}

	batch.clear								( );
}

} // namespace render