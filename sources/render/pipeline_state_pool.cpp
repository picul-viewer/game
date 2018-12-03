#include "pipeline_state_pool.h"
#include <macros.h>

namespace render {

namespace __render_pipeline_state_pool_detail {

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

bool operator==( rasterizer_state::cook const& l, rasterizer_state::cook const& r )
{
	return
		( l.desc.FillMode				== r.desc.FillMode				) &&
		( l.desc.CullMode				== r.desc.CullMode				) &&
		( l.desc.FrontCounterClockwise	== r.desc.FrontCounterClockwise	) &&
		( l.desc.DepthBias				== r.desc.DepthBias				) &&
		( l.desc.DepthBiasClamp			== r.desc.DepthBiasClamp		) &&
		( l.desc.SlopeScaledDepthBias	== r.desc.SlopeScaledDepthBias	) &&
		( l.desc.DepthClipEnable		== r.desc.DepthClipEnable		) &&	
		( l.desc.ScissorEnable			== r.desc.ScissorEnable			) &&
		( l.desc.MultisampleEnable		== r.desc.MultisampleEnable		) &&
		( l.desc.AntialiasedLineEnable	== r.desc.AntialiasedLineEnable	);
}

template<typename DataType>
typename DataType::data_type create( typename DataType::cook_type const& in_cook, DataType* in_data, u32 in_count )
{
	u32 i = 0;

	for ( ; in_data[i].state.get( ) != nullptr; ++i )
	{
		ASSERT( i < in_count );
		if ( in_data[i].cook == in_cook )
			return in_data[i].state;
	}

	in_data[i].cook	= in_cook;
	in_data[i].state.create( in_cook );

	return in_data[i].state;
}

} // namespace __render_pipeline_state_pool_detail

} // namespace render