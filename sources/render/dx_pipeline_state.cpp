#include "dx_pipeline_state.h"
#include "dx.h"

namespace render {

u32 const max_rt_count = 8;

void dx_pipeline_state::graphics_cook::create( )
{
	desc.PS.BytecodeLength = 0;
	desc.PS.pShaderBytecode = nullptr;
	desc.DS.BytecodeLength = 0;
	desc.DS.pShaderBytecode = nullptr;
	desc.HS.BytecodeLength = 0;
	desc.HS.pShaderBytecode = nullptr;
	desc.GS.BytecodeLength = 0;
	desc.GS.pShaderBytecode = nullptr;
	desc.StreamOutput.pSODeclaration = nullptr;
	desc.StreamOutput.NumEntries = 0;
	desc.StreamOutput.pBufferStrides = nullptr;
	desc.StreamOutput.NumStrides = 0;
	desc.StreamOutput.RasterizedStream = 0;
	set_blend( );
	set_blend_for_rt( 0 );
	desc.SampleMask = 0xFFFFFFFF;
	set_rasterizer_state( );
	set_depth( );
	set_stencil( );
	set_input_layout( 0, nullptr );
	desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
	set_rtv_count( 0 );
	for (u32 i = 0; i < max_rt_count; ++i)
		desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	set_dsv_format( DXGI_FORMAT_UNKNOWN );
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.NodeMask = 0;
	desc.CachedPSO.CachedBlobSizeInBytes = 0;
	desc.CachedPSO.pCachedBlob = nullptr;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
}

void dx_pipeline_state::graphics_cook::set_root_signature(
	dx_root_signature const& in_signature
)
{
	desc.pRootSignature = in_signature;
}

void dx_pipeline_state::graphics_cook::set_shader(
	shader_type const in_shader_type,
	pcvoid const in_data,
	uptr const in_size
)
{
	D3D12_SHADER_BYTECODE* shader = nullptr;

	switch ( in_shader_type )
	{
		case shader_type_vertex:	shader = &desc.VS; break;
		case shader_type_hull:		shader = &desc.HS; break;
		case shader_type_domain:	shader = &desc.DS; break;
		case shader_type_geometry:	shader = &desc.GS; break;
		case shader_type_pixel:		shader = &desc.PS; break;
		default: UNREACHABLE_CODE
	}

	shader->pShaderBytecode = in_data;
	shader->BytecodeLength = in_size;
}

void dx_pipeline_state::graphics_cook::set_blend(
	bool in_enable_independent_blend,
	bool in_enable_alpha_to_coverage
)
{
	desc.BlendState.AlphaToCoverageEnable = in_enable_alpha_to_coverage ? TRUE : FALSE;
	desc.BlendState.IndependentBlendEnable = in_enable_independent_blend ? TRUE : FALSE;
}

void dx_pipeline_state::graphics_cook::set_blend_for_rt(
	u32 const in_render_target_index,
	bool const in_enable_blending,
	bool const in_enable_logic_operations,
	D3D12_BLEND const in_source_blend,
	D3D12_BLEND const in_destination_blend,
	D3D12_BLEND_OP const in_blend_op,
	D3D12_BLEND const in_source_blend_alpha,
	D3D12_BLEND const in_destination_blend_alpha,
	D3D12_BLEND_OP const in_blend_op_alpha,
	D3D12_LOGIC_OP const in_logic_op,
	u8 const in_color_write_mask
)
{
	ASSERT_CMP( in_render_target_index, <, max_rt_count );

	desc.BlendState.RenderTarget[in_render_target_index].BlendEnable = in_enable_blending ? TRUE : FALSE;
	desc.BlendState.RenderTarget[in_render_target_index].LogicOpEnable = in_enable_logic_operations ? TRUE : FALSE;
	desc.BlendState.RenderTarget[in_render_target_index].SrcBlend = in_source_blend;
	desc.BlendState.RenderTarget[in_render_target_index].DestBlend = in_destination_blend;
	desc.BlendState.RenderTarget[in_render_target_index].BlendOp = in_blend_op;
	desc.BlendState.RenderTarget[in_render_target_index].SrcBlendAlpha = in_source_blend_alpha;
	desc.BlendState.RenderTarget[in_render_target_index].DestBlendAlpha = in_destination_blend_alpha;
	desc.BlendState.RenderTarget[in_render_target_index].BlendOpAlpha = in_blend_op_alpha;
	desc.BlendState.RenderTarget[in_render_target_index].LogicOp = in_logic_op;
	desc.BlendState.RenderTarget[in_render_target_index].RenderTargetWriteMask = in_color_write_mask;
}

void dx_pipeline_state::graphics_cook::set_rasterizer_state(
	D3D12_FILL_MODE	const in_fill_mode,
	D3D12_CULL_MODE	const in_cull_mode,
	bool const in_front_counter_clockwise,
	s32	const in_depth_bias,
	f32	const in_depth_bias_clamp,
	f32	const in_slope_scaled_depth_bias,
	bool const in_depth_clip_enable,
	bool const in_multisample_enable,
	bool const in_antialiased_line_enable,
	bool const in_conservative_rasterization_enabled
)
{
	desc.RasterizerState.FillMode = in_fill_mode;
	desc.RasterizerState.CullMode = in_cull_mode;
	desc.RasterizerState.FrontCounterClockwise = in_front_counter_clockwise ? TRUE : FALSE;
	desc.RasterizerState.DepthBias = in_depth_bias;
	desc.RasterizerState.DepthBiasClamp = in_depth_bias_clamp;
	desc.RasterizerState.SlopeScaledDepthBias = in_slope_scaled_depth_bias;
	desc.RasterizerState.DepthClipEnable = in_depth_clip_enable ? TRUE : FALSE;
	desc.RasterizerState.MultisampleEnable = in_multisample_enable ? TRUE : FALSE;
	desc.RasterizerState.AntialiasedLineEnable = in_antialiased_line_enable ? TRUE : FALSE;
	desc.RasterizerState.ForcedSampleCount = 0;
	desc.RasterizerState.ConservativeRaster =
		in_conservative_rasterization_enabled ?
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON :
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void dx_pipeline_state::graphics_cook::set_depth(
	bool const in_enable_depth,
	bool const in_write_depth,
	D3D12_COMPARISON_FUNC const in_comparison_func
)
{
	desc.DepthStencilState.DepthEnable = in_enable_depth ? TRUE : FALSE;
	desc.DepthStencilState.DepthWriteMask = in_write_depth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.DepthFunc = in_comparison_func;
}

void dx_pipeline_state::graphics_cook::set_stencil(
	bool const in_enable_stencil,
	u8 const in_stencil_read_mask,
	u8 const in_stencil_write_mask,
	D3D12_COMPARISON_FUNC const in_front_face_comparison_func,
	D3D12_STENCIL_OP const in_front_face_stencil_fail_op,
	D3D12_STENCIL_OP const in_front_face_stencil_depth_fail_op,
	D3D12_STENCIL_OP const in_front_face_stencil_pass_op,
	D3D12_COMPARISON_FUNC const in_back_face_comparison_func,
	D3D12_STENCIL_OP const in_back_face_stencil_fail_op,
	D3D12_STENCIL_OP const in_back_face_stencil_depth_fail_op,
	D3D12_STENCIL_OP const in_back_face_stencil_pass_op
)
{
	desc.DepthStencilState.StencilEnable = in_enable_stencil ? TRUE : FALSE;
	desc.DepthStencilState.StencilReadMask = in_stencil_read_mask;
	desc.DepthStencilState.StencilWriteMask = in_stencil_write_mask;
	desc.DepthStencilState.FrontFace.StencilFunc = in_front_face_comparison_func;
	desc.DepthStencilState.FrontFace.StencilFailOp = in_front_face_stencil_fail_op;
	desc.DepthStencilState.FrontFace.StencilDepthFailOp = in_front_face_stencil_depth_fail_op;
	desc.DepthStencilState.FrontFace.StencilPassOp = in_front_face_stencil_pass_op;
	desc.DepthStencilState.BackFace.StencilFunc = in_back_face_comparison_func;
	desc.DepthStencilState.BackFace.StencilFailOp = in_back_face_stencil_fail_op;
	desc.DepthStencilState.BackFace.StencilDepthFailOp = in_back_face_stencil_depth_fail_op;
	desc.DepthStencilState.BackFace.StencilPassOp = in_back_face_stencil_pass_op;
}

void dx_pipeline_state::graphics_cook::set_input_layout(
	u32 const in_element_count,
	D3D12_INPUT_ELEMENT_DESC const* const in_elements
)
{
	desc.InputLayout.NumElements = in_element_count;
	desc.InputLayout.pInputElementDescs = in_elements;
}

void dx_pipeline_state::graphics_cook::set_primitive_topology(
	D3D12_PRIMITIVE_TOPOLOGY_TYPE const in_topology
)
{
	desc.PrimitiveTopologyType = in_topology;
}

void dx_pipeline_state::graphics_cook::set_rtv_count(
	u32 const in_count
)
{
	ASSERT_CMP( in_count, <, max_rt_count );

	desc.NumRenderTargets = in_count;
}

void dx_pipeline_state::graphics_cook::set_rtv_format(
	u32 const in_index,
	DXGI_FORMAT const in_format
)
{
	ASSERT_CMP( in_index, <, max_rt_count );

	desc.RTVFormats[in_index] = in_format;
}

void dx_pipeline_state::graphics_cook::set_dsv_format(
	DXGI_FORMAT const in_format
)
{
	desc.DSVFormat = in_format;
}

void dx_pipeline_state::compute_cook::create( )
{
	desc.NodeMask = 0;
	desc.CachedPSO.CachedBlobSizeInBytes = 0;
	desc.CachedPSO.pCachedBlob = nullptr;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
}

void dx_pipeline_state::compute_cook::set_root_signature(
	dx_root_signature const& in_signature
)
{
	desc.pRootSignature = in_signature;
}

void dx_pipeline_state::compute_cook::set_shader(
	pcvoid const in_data,
	uptr const in_size
)
{
	desc.CS.pShaderBytecode = in_data;
	desc.CS.BytecodeLength = in_size;
}

void dx_pipeline_state::create( graphics_cook const& in_cook )
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateGraphicsPipelineState( &in_cook.desc, IID_PPV_ARGS( &m_pso ) ) );
}

void dx_pipeline_state::create( compute_cook const& in_cook )
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateComputePipelineState( &in_cook.desc, IID_PPV_ARGS( &m_pso ) ) );
}

void dx_pipeline_state::destroy( )
{
	s32 const ref_cnt = m_pso->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render