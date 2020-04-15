#include "dx_command_signature.h"
#include "dx.h"

namespace render {

void dx_command_signature::indirect_argument::create_draw( )
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
}

void dx_command_signature::indirect_argument::create_draw_indexed( )
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
}

void dx_command_signature::indirect_argument::create_dispatch( )
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
}

void dx_command_signature::indirect_argument::create_vertex_buffer_update(
	u32 const in_slot
)
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW;
	desc.VertexBuffer.Slot = in_slot;
}

void dx_command_signature::indirect_argument::create_index_buffer_update( )
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW;
}

void dx_command_signature::indirect_argument::create_constants_update(
	u32 const in_root_parameter_index,
	u32 const in_offset,
	u32 const in_count
)
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
	desc.Constant.RootParameterIndex = in_root_parameter_index;
	desc.Constant.DestOffsetIn32BitValues = in_offset;
	desc.Constant.Num32BitValuesToSet = in_count;
}

void dx_command_signature::indirect_argument::create_cbv_update(
	u32 const in_root_parameter_index
)
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	desc.ConstantBufferView.RootParameterIndex = in_root_parameter_index;
}

void dx_command_signature::indirect_argument::create_srv_update(
	u32 const in_root_parameter_index
)
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
	desc.ShaderResourceView.RootParameterIndex = in_root_parameter_index;
}

void dx_command_signature::indirect_argument::create_uav_update(
	u32 const in_root_parameter_index
)
{
	desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	desc.UnorderedAccessView.RootParameterIndex = in_root_parameter_index;
}

void dx_command_signature::cook::create(
	u32 const in_stride,
	u32 const in_argument_count,
	indirect_argument* const in_arguments
)
{
	desc.ByteStride = in_stride;
	desc.NumArgumentDescs = in_argument_count;
	desc.pArgumentDescs = &in_arguments[0].desc;
	desc.NodeMask = 0;

	need_rs = false;
	for (u32 i = 0; i < in_argument_count; ++i)
		need_rs |= in_arguments[i].desc.Type > D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
}

void dx_command_signature::create( cook const& in_cook, dx_root_signature const& in_root_signature )
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateCommandSignature( &in_cook.desc, in_cook.need_rs ? (ID3D12RootSignature*)in_root_signature : nullptr, IID_PPV_ARGS( &m_command_signature ) ) );
}

void dx_command_signature::destroy( )
{
	s32 const ref_cnt = m_command_signature->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render