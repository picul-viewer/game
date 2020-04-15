#ifndef GUARD_RENDER_DX_ROOT_SIGNATURE_H_INCLUDED
#define GUARD_RENDER_DX_ROOT_SIGNATURE_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

class dx_root_signature
{
public:
	struct descriptor_range
	{
		void create(
			D3D12_DESCRIPTOR_RANGE_TYPE const in_type,
			u32 const in_register,
			u32 const in_space,
			u32 const in_descriptor_count,
			u32 const in_range_offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		);

		D3D12_DESCRIPTOR_RANGE desc;
	};

	struct root_parameter
	{
		void create_constants(
			u32 const in_register,
			u32 const in_space,
			u32 const in_count,
			D3D12_SHADER_VISIBILITY const in_visibility
		);

		void create_descriptor(
			D3D12_ROOT_PARAMETER_TYPE const in_type,
			u32 const in_register,
			u32 const in_space,
			D3D12_SHADER_VISIBILITY const in_visibility
		);

		void create_descriptor_table(
			descriptor_range* const in_ranges,
			u32 const in_ranges_count,
			D3D12_SHADER_VISIBILITY const in_visibility
		);

		D3D12_ROOT_PARAMETER desc;
	};

	struct static_sampler
	{
		void create(
			u32 const in_register,
			u32 const in_space,
			D3D12_SHADER_VISIBILITY const in_visibility,
			D3D12_FILTER const in_filter,
			D3D12_TEXTURE_ADDRESS_MODE const in_address_u,
			D3D12_TEXTURE_ADDRESS_MODE const in_address_v,
			D3D12_TEXTURE_ADDRESS_MODE const in_address_w,
			u32 const in_max_anisotrophy = 1,
			D3D12_STATIC_BORDER_COLOR const in_border_color = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
			D3D12_COMPARISON_FUNC const in_comparison_func = D3D12_COMPARISON_FUNC_ALWAYS,
			float const in_min_lod = 0.0f,
			float const in_max_lod = D3D12_FLOAT32_MAX,
			float const in_mip_lod_bias = 0.0f
		);

		D3D12_STATIC_SAMPLER_DESC desc;
	};

	struct cook
	{
		void create(
			u32 const in_root_parameter_count,
			root_parameter* const in_root_parameters,
			u32 const in_static_sampler_count,
			static_sampler* const in_static_samplers,
			bool const in_allow_ia = false,
			bool const in_allow_vs = false,
			bool const in_allow_hs = false,
			bool const in_allow_ds = false,
			bool const in_allow_gs = false,
			bool const in_allow_ps = false,
			bool const in_allow_so = false
		);

		D3D12_ROOT_SIGNATURE_DESC desc;
	};

	void create( cook const& in_cook );
	void destroy( );

	inline void set( ID3D12RootSignature* const in_rs ) { m_rs = in_rs; }

	inline operator ID3D12RootSignature*( ) const { return m_rs; }
	inline ID3D12RootSignature* operator->( ) const { return m_rs; }

private:
	ID3D12RootSignature* m_rs;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_ROOT_SIGNATURE_H_INCLUDED