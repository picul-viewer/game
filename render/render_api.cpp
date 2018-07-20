#include "render_api.h"
#include "render_parameters.h"

#include <macros.h>

namespace render {

const DXGI_FORMAT api::backbuffer_pixel_format = DXGI_FORMAT_R8G8B8A8_UNORM;

void api::create( )
{
	UINT create_device_flags = g_parameters.is_d3d_debug ? D3D11_CREATE_DEVICE_DEBUG : 0;

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
	swap_chain_desc.BufferDesc.Width					= g_parameters.screen_resolution.x;
	swap_chain_desc.BufferDesc.Height					= g_parameters.screen_resolution.y;
	swap_chain_desc.BufferDesc.Format					= backbuffer_pixel_format;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator	= 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator	= 1;
	swap_chain_desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow						= (HWND)g_parameters.hwnd;
	swap_chain_desc.SampleDesc.Count					= 1;
	swap_chain_desc.SampleDesc.Quality					= 0;
	swap_chain_desc.Windowed							= (BOOL)g_parameters.is_windowed;

	HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		create_device_flags, feature_levels, feature_levels_count, D3D11_SDK_VERSION,
		&swap_chain_desc, &m_swap_chain, &m_device, &m_feature_level, &m_context );

	if ( hr != S_OK )
		LOG( "render device and swap chain were not created!" );

	m_swap_chain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&m_backbuffer );
}

void api::destroy( )
{
	dx_release( m_backbuffer );
	dx_release( m_swap_chain );
	dx_release( m_context );
	dx_release( m_device );
}

ID3D11Device* api::get_device( )
{
	return m_device;
}

IDXGISwapChain* api::get_swap_chain( )
{
	return m_swap_chain;
}

ID3D11DeviceContext* api::get_context( )
{
	return m_context;
}

ID3D11Texture2D* api::get_backbuffer( )
{
	return m_backbuffer;
}

D3D_FEATURE_LEVEL api::get_feature_level( )
{
	return m_feature_level;
}


api g_api;

} // namespace render