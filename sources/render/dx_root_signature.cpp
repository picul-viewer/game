#include "dx_root_signature.h"
#include "dx.h"

namespace render {

u32 const max_rt_count = 8;

void dx_root_signature::descriptor_range::create(
	D3D12_DESCRIPTOR_RANGE_TYPE const in_type,
	u32 const in_register,
	u32 const in_space,
	u32 const in_descriptor_count,
	u32 const in_range_offset
)
{
	desc.RangeType = in_type;
	desc.NumDescriptors = in_descriptor_count;
	desc.BaseShaderRegister = in_register;
	desc.RegisterSpace = in_space;
	desc.OffsetInDescriptorsFromTableStart = in_range_offset;
}

void dx_root_signature::root_parameter::create_constants(
	u32 const in_register,
	u32 const in_space,
	u32 const in_count,
	D3D12_SHADER_VISIBILITY const in_visibility
)
{
	desc.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	desc.Constants.Num32BitValues = in_count;
	desc.Constants.ShaderRegister = in_register;
	desc.Constants.RegisterSpace = in_space;
	desc.ShaderVisibility = in_visibility;
}

void dx_root_signature::root_parameter::create_descriptor(
	D3D12_ROOT_PARAMETER_TYPE const in_type,
	u32 const in_register,
	u32 const in_space,
	D3D12_SHADER_VISIBILITY const in_visibility
)
{
	ASSERT(
		( in_type == D3D12_ROOT_PARAMETER_TYPE_CBV ) |
		( in_type == D3D12_ROOT_PARAMETER_TYPE_SRV ) |
		( in_type == D3D12_ROOT_PARAMETER_TYPE_UAV )
	);

	desc.ParameterType = in_type;
	desc.Descriptor.ShaderRegister = in_register;
	desc.Descriptor.RegisterSpace = in_space;
	desc.ShaderVisibility = in_visibility;
}

void dx_root_signature::root_parameter::create_descriptor_table(
	descriptor_range* const in_ranges,
	u32 const in_ranges_count,
	D3D12_SHADER_VISIBILITY const in_visibility
)
{
	desc.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.DescriptorTable.pDescriptorRanges = &in_ranges[0].desc;
	desc.DescriptorTable.NumDescriptorRanges = in_ranges_count;
	desc.ShaderVisibility = in_visibility;
}

void dx_root_signature::static_sampler::create(
	u32 const in_register,
	u32 const in_space,
	D3D12_SHADER_VISIBILITY const in_visibility,
	D3D12_FILTER const in_filter,
	D3D12_TEXTURE_ADDRESS_MODE const in_address_u,
	D3D12_TEXTURE_ADDRESS_MODE const in_address_v,
	D3D12_TEXTURE_ADDRESS_MODE const in_address_w,
	u32 const in_max_anisotrophy,
	D3D12_STATIC_BORDER_COLOR const in_border_color,
	D3D12_COMPARISON_FUNC const in_comparison_func,
	float const in_min_lod,
	float const in_max_lod,
	float const in_mip_lod_bias

)
{
	desc.Filter = in_filter;
	desc.AddressU = in_address_u;
	desc.AddressV = in_address_v;
	desc.AddressW = in_address_w;
	desc.MipLODBias = in_mip_lod_bias;
	desc.MaxAnisotropy = in_max_anisotrophy;
	desc.ComparisonFunc = in_comparison_func;
	desc.BorderColor = in_border_color;
	desc.MinLOD = in_min_lod;
	desc.MaxLOD = in_max_lod;
	desc.ShaderRegister = in_register;
	desc.RegisterSpace = in_space;
	desc.ShaderVisibility = in_visibility;
}

void dx_root_signature::cook::create(
	u32 const in_root_parameter_count,
	root_parameter* const in_root_parameters,
	u32 const in_static_sampler_count,
	static_sampler* const in_static_samplers,
	bool const in_allow_ia,
	bool const in_allow_vs,
	bool const in_allow_hs,
	bool const in_allow_ds,
	bool const in_allow_gs,
	bool const in_allow_ps,
	bool const in_allow_so
)
{
	desc.NumParameters = in_root_parameter_count;
	desc.pParameters = &in_root_parameters[0].desc;
	desc.NumStaticSamplers = in_static_sampler_count;
	desc.pStaticSamplers = &in_static_samplers[0].desc;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE |
		( in_allow_ia ? D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT : D3D12_ROOT_SIGNATURE_FLAG_NONE ) |
		( in_allow_vs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS ) |
		( in_allow_hs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS ) |
		( in_allow_ds ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS ) |
		( in_allow_gs ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS ) |
		( in_allow_ps ? D3D12_ROOT_SIGNATURE_FLAG_NONE : D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS ) |
		( in_allow_so ? D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT : D3D12_ROOT_SIGNATURE_FLAG_NONE );
}

void dx_root_signature::create( cook const& in_cook )
{
	ID3DBlob* rs_blob = nullptr;
	ID3DBlob* error_blob;

	{
		HRESULT const r = D3D12SerializeRootSignature( &in_cook.desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rs_blob, &error_blob );
		if ( FAILED( r ) )
			FATAL_ERROR( "dx12 error, reason is:\n%s", (pcstr)error_blob->GetBufferPointer( ) );
	}

	DX12_CHECK_RESULT( g_dx.device( )->CreateRootSignature( 0, rs_blob->GetBufferPointer( ), rs_blob->GetBufferSize( ), IID_PPV_ARGS( &m_rs ) ) );
	
	rs_blob->Release( );
	if ( error_blob )
		error_blob->Release( );
}

void dx_root_signature::destroy( )
{
	m_rs->Release( );
}

} // namespace render