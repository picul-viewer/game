#include "sampler_colletion.h"
#include "render_api.h"

namespace render {

namespace sampler_collection {

struct
{
	sampler m_texture_sampler;
	sampler m_point_sampler;
} m_data;
	
void create( )
{
	sampler::cook texture_sampler_cook;
	texture_sampler_cook.set_sampler(	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP );
	m_data.m_texture_sampler.create( texture_sampler_cook );

	sampler::cook accumulator_sampler_cook;
	accumulator_sampler_cook.set_sampler(	D3D11_FILTER_MIN_MAG_MIP_POINT,
											D3D11_TEXTURE_ADDRESS_CLAMP,
											D3D11_TEXTURE_ADDRESS_CLAMP,
											D3D11_TEXTURE_ADDRESS_CLAMP );
	m_data.m_point_sampler.create( accumulator_sampler_cook );
}

void destroy( )
{
	m_data.m_texture_sampler.destroy( );
	m_data.m_point_sampler.destroy( );
}
	
sampler const& get_texture_sampler( )
{
	return m_data.m_texture_sampler;
}

sampler const& get_point_sampler( )
{
	return m_data.m_point_sampler;
}

void bind_vs( )
{
	api::get_context( )->VSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

void bind_ps( )
{
	api::get_context( )->PSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

void bind_gs( )
{
	api::get_context( )->GSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

void bind_hs( )
{
	api::get_context( )->HSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

void bind_ds( )
{
	api::get_context( )->DSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

void bind_cs( )
{
	api::get_context( )->CSSetSamplers( 0, samplers_count, (ID3D11SamplerState**)&m_data );
}

} // namespace sampler_colletion

} // namespace render