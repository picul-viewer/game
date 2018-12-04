#ifndef __render_sampler_h_included_
#define __render_sampler_h_included_

#include <types.h>
#include "dx_include.h"

#include <math/vector.h>

namespace render {

class sampler
{
public:
	sampler( );

	struct cook
	{
		void set_sampler( D3D11_FILTER					in_filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR,
						  D3D11_TEXTURE_ADDRESS_MODE	in_address_u		= D3D11_TEXTURE_ADDRESS_CLAMP,
						  D3D11_TEXTURE_ADDRESS_MODE	in_address_v		= D3D11_TEXTURE_ADDRESS_CLAMP,
						  D3D11_TEXTURE_ADDRESS_MODE	in_address_w		= D3D11_TEXTURE_ADDRESS_CLAMP,
						  f32							in_mip_lod_bias		= 0.0f,
						  u32							in_max_anisotropy	= 1,
						  D3D11_COMPARISON_FUNC			in_comparison_func	= D3D11_COMPARISON_NEVER,
						  math::float4					in_border_color		= math::float4( 1.0f, 1.0f, 1.0f, 1.0f ),
						  f32							in_min_lod			= -D3D11_FLOAT32_MAX,
						  f32							in_max_lod			= D3D11_FLOAT32_MAX );

		D3D11_SAMPLER_DESC sampler_desc;
	};
	
	void create( cook const& in_cook );
	void destroy( );
	
	inline void set( ID3D11SamplerState* in_state ) { m_sampler = in_state; }
	inline ID3D11SamplerState* const& get( ) { return m_sampler; }

	void bind_vs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_ps( u32 in_slot, u32 in_count = 1 ) const;
	void bind_gs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_hs( u32 in_slot, u32 in_count = 1 ) const;
	void bind_ds( u32 in_slot, u32 in_count = 1 ) const;
	void bind_cs( u32 in_slot, u32 in_count = 1 ) const;

protected:
	ID3D11SamplerState* m_sampler;
};

} // namespace render

#endif // #ifndef __render_sampler_h_included_