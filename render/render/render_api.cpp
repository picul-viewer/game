#include "render_api.h"
#include "render_parameters.h"

namespace render {

namespace api
{

struct data
{
	ID3D11Device*			m_device;
	IDXGISwapChain*			m_swap_chain;
	ID3D11DeviceContext*	m_context;
	ID3D11Texture2D*		m_backbuffer;
	D3D_FEATURE_LEVEL		m_feature_level;
} m_data;

void create( )//HWND in_hwnd, math::u16x2 in_resolution, DXGI_FORMAT in_pixel_format, bool in_is_windowed, bool in_allow_debug )
{
	UINT create_device_flags = parameters::get_debug( ) ? D3D11_CREATE_DEVICE_DEBUG : 0;

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	u32 feature_levels_count = ARRAYSIZE( feature_levels );

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount							= 1;
	swap_chain_desc.BufferDesc.Width					= parameters::get_resolution( ).x;
	swap_chain_desc.BufferDesc.Height					= parameters::get_resolution( ).y;
	swap_chain_desc.BufferDesc.Format					= backbuffer_pixel_format;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator	= 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator	= 1;
	swap_chain_desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow						= parameters::get_hwnd( );
	swap_chain_desc.SampleDesc.Count					= 1;
	swap_chain_desc.SampleDesc.Quality					= 0;
	swap_chain_desc.Windowed							= (BOOL)parameters::get_windowed( );

	HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		create_device_flags, feature_levels, feature_levels_count, D3D11_SDK_VERSION,
		&swap_chain_desc, &m_data.m_swap_chain, &m_data.m_device, &m_data.m_feature_level, &m_data.m_context );

	if ( hr != S_OK )
		LOG( "render device and swap chain were not created!" );

	m_data.m_swap_chain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&m_data.m_backbuffer );
}

void destroy( )
{
	dx_release( m_data.m_backbuffer );
	dx_release( m_data.m_swap_chain );
	dx_release( m_data.m_context );
	dx_release( m_data.m_device );
}

ID3D11Device* get_device( )
{
	return m_data.m_device;
}

IDXGISwapChain* get_swap_chain( )
{
	return m_data.m_swap_chain;
}

ID3D11DeviceContext* get_context( )
{
	return m_data.m_context;
}

ID3D11Texture2D* get_backbuffer( )
{
	return m_data.m_backbuffer;
}

D3D_FEATURE_LEVEL get_feature_level( )
{
	return m_data.m_feature_level;
}

} // namespace render_api

} // namespace render