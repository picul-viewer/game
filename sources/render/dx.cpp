#include "dx.h"
#include <lib/fixed_string.h>
#include "parameters.h"

namespace render {

IDXGIAdapter* choose_adapter( IDXGIFactory1* const in_factory )
{
	IDXGIAdapter* adapter = nullptr;

	u32 i = 0;
	IDXGIAdapter* current_adapter = nullptr;

	// Use last GPU (but not software driver).
	while ( in_factory->EnumAdapters( i, &current_adapter ) != DXGI_ERROR_NOT_FOUND )
	{
		DXGI_ADAPTER_DESC desc;
		current_adapter->GetDesc( &desc );

		// Choose if NVidia or AMD.
		if ( ( desc.Description[0] == L'N' ) || ( desc.Description[0] == L'A' ) )
		{
			adapter = current_adapter;
			LOG( "Render device: %S\n", desc.Description );
			break;
		}

		++i;
	}

	return adapter;
}

void dx::create( )
{
#ifdef RENDER_ALLOW_DEBUG
	{
		ID3D12Debug* debug_controller;
		DX12_CHECK_RESULT( D3D12GetDebugInterface( IID_PPV_ARGS( &debug_controller ) ) );

		debug_controller->EnableDebugLayer( );
	}
#endif // #ifdef RENDER_ALLOW_DEBUG

	IDXGIFactory4* factory;
	DX12_CHECK_RESULT( CreateDXGIFactory1( IID_PPV_ARGS( &factory ) ) );

	IDXGIAdapter* const adapter = choose_adapter( factory );
	ASSERT( adapter );

	DX12_CHECK_RESULT( D3D12CreateDevice( adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS( &m_device ) ) );

	m_rtv_descriptor_size			= m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	m_dsv_descriptor_size			= m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
	m_cbv_srv_uav_descriptor_size	= m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
	m_sampler_descriptor_size		= m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );

	{
		D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
		command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		DX12_CHECK_RESULT( m_device->CreateCommandQueue( &command_queue_desc, IID_PPV_ARGS( &m_queue_graphics ) ) );
		set_dx_name( m_queue_graphics, "graphics_queue" );
	}

	{
		D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
		command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		DX12_CHECK_RESULT( m_device->CreateCommandQueue( &command_queue_desc, IID_PPV_ARGS( &m_queue_compute ) ) );
		set_dx_name( m_queue_compute, "compute_queue" );
	}

	{
		D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
		command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		DX12_CHECK_RESULT( m_device->CreateCommandQueue( &command_queue_desc, IID_PPV_ARGS( &m_queue_copy ) ) );
		set_dx_name( m_queue_copy, "copy_queue" );
	}

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	swap_chain_desc.BufferDesc.Width = g_parameters.screen_resolution.x;
	swap_chain_desc.BufferDesc.Height = g_parameters.screen_resolution.y;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferDesc.Format = back_buffer_format;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = swap_chain_buffer_count;
	swap_chain_desc.OutputWindow = (HWND)g_parameters.hwnd;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DX12_CHECK_RESULT( factory->CreateSwapChain( m_queue_graphics, &swap_chain_desc, (IDXGISwapChain**)&m_swap_chain ) );

	for ( u32 i = 0; i < swap_chain_buffer_count; ++i )
	{
		DX12_CHECK_RESULT( m_swap_chain->GetBuffer( i, IID_PPV_ARGS( &m_swap_chain_buffers[i] ) ) );
		set_dx_name( m_swap_chain_buffers[i], format( "swap_chain_buffer #%d", i ) );
	}

	factory->Release( );
}

void dx::destroy( )
{
	m_queue_graphics->Release( );
	m_queue_compute->Release( );
	m_queue_copy->Release( );
	for ( u32 i = 0; i < swap_chain_buffer_count; ++i )
		m_swap_chain_buffers[i]->Release( );
	m_swap_chain->Release( );
	m_device->Release( );

#ifdef RENDER_ALLOW_DEBUG
	{
		IDXGIDebug1* dxgi_debug = nullptr;
		DX12_CHECK_RESULT( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &dxgi_debug ) ) );

		dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		dxgi_debug->Release();
	}
#endif // #ifdef RENDER_ALLOW_DEBUG
}

dx g_dx;

} // namespace render