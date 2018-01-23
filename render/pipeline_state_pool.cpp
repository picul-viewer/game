#include "pipeline_state_pool.h"

namespace render {

bool operator==( depth_stencil_state::cook const& l, depth_stencil_state::cook const& r )
{
	return
		( l.desc.DepthEnable					== r.desc.DepthEnable					) &&
		( l.desc.DepthWriteMask					== r.desc.DepthWriteMask				) &&
		( l.desc.DepthFunc						== r.desc.DepthFunc						) &&
		( l.desc.StencilEnable					== r.desc.StencilEnable					) &&
		( l.desc.StencilReadMask				== r.desc.StencilReadMask				) &&
		( l.desc.StencilWriteMask				== r.desc.StencilWriteMask				) &&
		( l.desc.FrontFace.StencilFunc			== r.desc.FrontFace.StencilFunc			) &&
		( l.desc.FrontFace.StencilFailOp		== r.desc.FrontFace.StencilFailOp		) &&
		( l.desc.FrontFace.StencilDepthFailOp	== r.desc.FrontFace.StencilDepthFailOp	) &&
		( l.desc.FrontFace.StencilPassOp		== r.desc.FrontFace.StencilPassOp		) &&
		( l.desc.BackFace.StencilFunc			== r.desc.BackFace.StencilFunc			) &&
		( l.desc.BackFace.StencilFailOp			== r.desc.BackFace.StencilFailOp		) &&
		( l.desc.BackFace.StencilDepthFailOp	== r.desc.BackFace.StencilDepthFailOp	) &&
		( l.desc.BackFace.StencilPassOp			== r.desc.BackFace.StencilPassOp		);
}

template<u32 DS_max, u32 B_max, u32 R_max>
depth_stencil_state* pipeline_state_pool<DS_max, B_max, R_max>::create_depth_stencil( depth_stencil_state::cook const& in_cook )
{
	u32 i = 0;

	for ( ; ( i < DS_max ) && ( m_depth_stencil_data[i].state != nullptr ); ++i )
		if ( m_depth_stencil_data[i].cook == in_cook )
			return &m_depth_stencil_data[i].state;

	ASSERT( i < DS_max );
	m_depth_stencil_data[i].cook	= in_cook;
	m_depth_stencil_data[i].state.create( in_cook );

	return &m_depth_stencil_data[i].state;
}

bool operator==( D3D11_RENDER_TARGET_BLEND_DESC const& l, D3D11_RENDER_TARGET_BLEND_DESC const& r )
{
	return
		( l.BlendEnable				== r.BlendEnable			) &&
		( l.SrcBlend				== r.SrcBlend				) &&
		( l.DestBlend				== r.DestBlend				) &&
		( l.BlendOp					== r.BlendOp				) &&
		( l.SrcBlendAlpha			== r.SrcBlendAlpha			) &&
		( l.DestBlendAlpha			== r.DestBlendAlpha			) &&
		( l.BlendOpAlpha			== r.BlendOpAlpha			) &&
		( l.RenderTargetWriteMask	== r.RenderTargetWriteMask	);
}

bool operator==( blend_state::cook const& l, blend_state::cook const& r )
{
	if (	( l.desc.AlphaToCoverageEnable	!= r.desc.AlphaToCoverageEnable ) ||
			( l.desc.IndependentBlendEnable	!= r.desc.IndependentBlendEnable ) )
		return false;

	if ( l.desc.IndependentBlendEnable != 0 )
	{
		return
			( l.desc.RenderTarget[0] == r.desc.RenderTarget[0] ) &&
			( l.desc.RenderTarget[1] == r.desc.RenderTarget[1] ) &&
			( l.desc.RenderTarget[2] == r.desc.RenderTarget[2] ) &&
			( l.desc.RenderTarget[3] == r.desc.RenderTarget[3] ) &&
			( l.desc.RenderTarget[4] == r.desc.RenderTarget[4] ) &&
			( l.desc.RenderTarget[5] == r.desc.RenderTarget[5] ) &&
			( l.desc.RenderTarget[6] == r.desc.RenderTarget[6] ) &&
			( l.desc.RenderTarget[7] == r.desc.RenderTarget[7] );
	}
	else
	{
		return l.desc.RenderTarget[0] == r.desc.RenderTarget[0];
	}
}

template<u32 DS_max, u32 B_max, u32 R_max>
blend_state* pipeline_state_pool<DS_max, B_max, R_max>::create_blend( blend_state::cook const& in_cook )
{
	u32 i = 0;

	for ( ; ( i < B_max ) && ( m_blend_data[i].state != nullptr ); ++i )
		if ( m_blend_data[i].cook == in_cook )
			return &m_blend_data[i].state;

	ASSERT( i < B_max );
	m_blend_data[i].cook	= in_cook;
	m_blend_data[i].state.create( in_cook );

	return &m_blend_data[i].state;
}

bool operator==( rasterizer_state::cook const& l, rasterizer_state::cook const& r )
{
	return
		( l.desc.FillMode				== l.desc.FillMode				) &&
		( l.desc.CullMode				== l.desc.CullMode				) &&
		( l.desc.FrontCounterClockwise	== l.desc.FrontCounterClockwise	) &&
		( l.desc.DepthBias				== l.desc.DepthBias				) &&
		( l.desc.DepthBiasClamp			== l.desc.DepthBiasClamp		) &&
		( l.desc.SlopeScaledDepthBias	== l.desc.SlopeScaledDepthBias	) &&
		( l.desc.DepthClipEnable		== l.desc.DepthClipEnable		) &&	
		( l.desc.ScissorEnable			== l.desc.ScissorEnable			) &&
		( l.desc.MultisampleEnable		== l.desc.MultisampleEnable		) &&
		( l.desc.AntialiasedLineEnable	== l.desc.AntialiasedLineEnable	);
}

template<u32 DS_max, u32 B_max, u32 R_max>
rasterizer_state* pipeline_state_pool<DS_max, B_max, R_max>::create_rasterizer( rasterizer_state::cook const& in_cook )
{
	u32 i = 0;

	for ( ; ( i < R_max ) && ( m_rasterizer_data[i].state != nullptr ); ++i )
		if ( m_rasterizer_data[i].cook == in_cook )
			return &m_rasterizer_data[i].state;

	ASSERT( i < R_max );
	m_rasterizer_data[i].cook	= in_cook;
	m_rasterizer_data[i].state.create( in_cook );

	return &m_rasterizer_data[i].state;
}

template<u32 DS_max, u32 B_max, u32 R_max>
void pipeline_state_pool<DS_max, B_max, R_max>::destroy( )
{
	for ( u32 i = 0; ( i < c_DS_max ) && ( m_depth_stencil_data[i].state != nullptr ); ++i )
		m_depth_stencil_data[i].state.destroy( );
	for ( u32 i = 0; ( i < c_B_max ) && ( m_blend_data[i].state != nullptr ); ++i )
		m_blend_data[i].state.destroy( );
	for ( u32 i = 0; ( i < c_R_max ) && ( m_rasterizer_data[i].state != nullptr ); ++i )
		m_rasterizer_data[i].state.destroy( );
}

}