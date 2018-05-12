#include "pipeline_state.h"
#include "render_api.h"

namespace render {

void depth_stencil_state::cook::set_depth(	bool					in_enable_depth,
											bool					in_write_depth,
											D3D11_COMPARISON_FUNC	in_comparison_func )
{
	desc.DepthEnable	= in_enable_depth;
	desc.DepthWriteMask	= in_write_depth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc		= in_comparison_func;
}

void depth_stencil_state::cook::set_stencil(	bool					in_enable_stencil,
												u8						in_stencil_read_mask,
												u8						in_stencil_write_mask,
												D3D11_COMPARISON_FUNC	in_front_face_comparison_func,
												D3D11_STENCIL_OP		in_front_face_stencil_fail_op,
												D3D11_STENCIL_OP		in_front_face_stencil_depth_fail_op,
												D3D11_STENCIL_OP		in_front_face_stencil_pass_op,
												D3D11_COMPARISON_FUNC	in_back_face_comparison_func,
												D3D11_STENCIL_OP		in_back_face_stencil_fail_op,
												D3D11_STENCIL_OP		in_back_face_stencil_depth_fail_op,
												D3D11_STENCIL_OP		in_back_face_stencil_pass_op )
{
	desc.StencilEnable					= in_enable_stencil;
	desc.StencilReadMask				= in_stencil_read_mask;
	desc.StencilWriteMask				= in_stencil_write_mask;
	desc.FrontFace.StencilFunc			= in_front_face_comparison_func;
	desc.FrontFace.StencilFailOp		= in_front_face_stencil_fail_op;
	desc.FrontFace.StencilDepthFailOp	= in_front_face_stencil_depth_fail_op;
	desc.FrontFace.StencilPassOp		= in_front_face_stencil_pass_op;
	desc.BackFace.StencilFunc			= in_back_face_comparison_func;
	desc.BackFace.StencilFailOp			= in_back_face_stencil_fail_op;
	desc.BackFace.StencilDepthFailOp	= in_back_face_stencil_depth_fail_op;
	desc.BackFace.StencilPassOp			= in_back_face_stencil_pass_op;
}

depth_stencil_state::depth_stencil_state( ) :
	m_depth_stencil_state( nullptr )
{ }

void depth_stencil_state::create( cook const& in_cook )
{
	api::get_device( )->CreateDepthStencilState( &in_cook.desc, &m_depth_stencil_state );
}

void depth_stencil_state::destroy( )
{
	dx_release( m_depth_stencil_state );
}

void depth_stencil_state::bind( u8 in_stencil_ref ) const
{
	api::get_context( )->OMSetDepthStencilState( m_depth_stencil_state, in_stencil_ref );
}


void blend_state::cook::set_blend(	bool in_enable_alpha_to_coverage,
									bool in_enable_independent_blend )
{
	desc.AlphaToCoverageEnable	= in_enable_alpha_to_coverage;
	desc.IndependentBlendEnable	= in_enable_independent_blend;
}

void blend_state::cook::set_blend_for_rt(	u32				in_render_target_index,
											bool			in_enable_blending,
											D3D11_BLEND		in_source_blend,
											D3D11_BLEND		in_destination_blend,
											D3D11_BLEND_OP	in_blend_op,
											D3D11_BLEND		in_source_blend_alpha,
											D3D11_BLEND		in_destination_blend_alpha,
											D3D11_BLEND_OP	in_blend_op_alpha,
											u8				in_color_write_mask )
{
	desc.RenderTarget[in_render_target_index].BlendEnable			= in_enable_blending;
	desc.RenderTarget[in_render_target_index].SrcBlend				= in_source_blend;
	desc.RenderTarget[in_render_target_index].DestBlend				= in_destination_blend;
	desc.RenderTarget[in_render_target_index].BlendOp				= in_blend_op;
	desc.RenderTarget[in_render_target_index].SrcBlendAlpha			= in_source_blend_alpha;
	desc.RenderTarget[in_render_target_index].DestBlendAlpha		= in_destination_blend_alpha;
	desc.RenderTarget[in_render_target_index].BlendOpAlpha			= in_blend_op_alpha;
	desc.RenderTarget[in_render_target_index].RenderTargetWriteMask	= in_color_write_mask;
}

blend_state::blend_state( ) :
	m_blend_state( nullptr )
{ }

void blend_state::create( cook const& in_cook )
{
	api::get_device( )->CreateBlendState( &in_cook.desc, &m_blend_state );
}

void blend_state::destroy( )
{
	dx_release( m_blend_state );
}

void blend_state::bind( math::float4 const& in_blend_factor, u32 in_sample_mask ) const
{
	api::get_context( )->OMSetBlendState( m_blend_state, in_blend_factor.data, in_sample_mask );
}


void rasterizer_state::cook::set(	D3D11_FILL_MODE	in_fill_mode,
									D3D11_CULL_MODE	in_cull_mode,
									bool			in_front_counter_clockwise,
									s32				in_depth_bias,
									f32				in_depth_bias_clamp,
									f32				in_slope_scaled_depth_bias,
									bool			in_depth_clip_enable,
									bool			in_scissor_enable,
									bool			in_multisample_enable,
									bool			in_antialiased_line_enable )
{
	desc.FillMode				= in_fill_mode;
	desc.CullMode				= in_cull_mode;
	desc.FrontCounterClockwise	= in_front_counter_clockwise;
	desc.DepthBias				= in_depth_bias;
	desc.DepthBiasClamp			= in_depth_bias_clamp;
	desc.SlopeScaledDepthBias	= in_slope_scaled_depth_bias;
	desc.DepthClipEnable		= in_depth_clip_enable;
	desc.ScissorEnable			= in_scissor_enable;
	desc.MultisampleEnable		= in_multisample_enable;
	desc.AntialiasedLineEnable	= in_antialiased_line_enable;
}

rasterizer_state::rasterizer_state( ) :
	m_rasterizer_state( nullptr )
{ }

void rasterizer_state::create( cook const& in_cook )
{
	api::get_device( )->CreateRasterizerState( &in_cook.desc, &m_rasterizer_state );
}

void rasterizer_state::destroy( )
{
	dx_release( m_rasterizer_state );
}

void rasterizer_state::bind( ) const
{
	api::get_context( )->RSSetState( m_rasterizer_state );
}

} // namespace render