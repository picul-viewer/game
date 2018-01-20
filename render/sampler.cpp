#include "sampler.h"
#include "render_api.h"

namespace render {

sampler::sampler( ) :
	m_sampler( nullptr )
{ }

void sampler::cook::set_sampler( D3D11_FILTER				in_filter,
								 D3D11_TEXTURE_ADDRESS_MODE	in_address_u,
								 D3D11_TEXTURE_ADDRESS_MODE	in_address_v,
								 D3D11_TEXTURE_ADDRESS_MODE	in_address_w,
								 f32						in_mip_lod_bias,
								 u32						in_max_anisotropy,
								 D3D11_COMPARISON_FUNC		in_comparison_func,
								 math::float4				in_border_color,
								 f32						in_min_lod,
								 f32						in_max_lod )
{
	sampler_desc.Filter				= in_filter;
	sampler_desc.AddressU			= in_address_u;
	sampler_desc.AddressV			= in_address_v;
	sampler_desc.AddressW			= in_address_w;
	sampler_desc.MipLODBias			= in_mip_lod_bias;
	sampler_desc.MaxAnisotropy		= in_max_anisotropy;
	sampler_desc.ComparisonFunc		= in_comparison_func;
	sampler_desc.BorderColor[0]		= in_border_color.x;
	sampler_desc.BorderColor[1]		= in_border_color.y;
	sampler_desc.BorderColor[2]		= in_border_color.z;
	sampler_desc.BorderColor[3]		= in_border_color.w;
	sampler_desc.MinLOD				= in_min_lod;
	sampler_desc.MaxLOD				= in_max_lod;
}

void sampler::create( cook const& in_cook )
{
	api::get_device( )->CreateSamplerState( &in_cook.sampler_desc, &m_sampler );
}

void sampler::set( ID3D11SamplerState* in_state )
{
	m_sampler = in_state;
}

void sampler::destroy( )
{
	dx_release( m_sampler );
}

void sampler::bind_vs( u32 in_slot ) const
{
	api::get_context( )->VSSetSamplers( in_slot, 1, &m_sampler );
}

void sampler::bind_ps( u32 in_slot ) const
{
	api::get_context( )->PSSetSamplers( in_slot, 1, &m_sampler );
}

void sampler::bind_gs( u32 in_slot ) const
{
	api::get_context( )->GSSetSamplers( in_slot, 1, &m_sampler );
}

void sampler::bind_hs( u32 in_slot ) const
{
	api::get_context( )->HSSetSamplers( in_slot, 1, &m_sampler );
}

void sampler::bind_ds( u32 in_slot ) const
{
	api::get_context( )->DSSetSamplers( in_slot, 1, &m_sampler );
}

void sampler::bind_cs( u32 in_slot ) const
{
	api::get_context( )->CSSetSamplers( in_slot, 1, &m_sampler );
}

} // namespace render