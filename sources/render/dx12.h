#ifndef GUARD_RENDER_DX12_H_INCLUDED
#define GUARD_RENDER_DX12_H_INCLUDED

#pragma warning(push)
#pragma warning(disable:4005)

#include <macros.h>

#include <dxgi1_4.h>
#include <d3d12.h>

#pragma warning(pop)

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define DX12_CHECK_RESULT( expr )								\
{																\
	HRESULT const r = (expr);									\
	if ( FAILED( r ) )											\
		FATAL_ERROR( "dx12 error, result was 0x%08llx", r );	\
}																\

namespace render {

void set_dx_name( ID3D12Object* const in_object, pcstr const in_name );

DXGI_FORMAT format_get_typeless( DXGI_FORMAT const in_format );
u32 format_get_bits_per_pixel( DXGI_FORMAT const in_format );
DXGI_FORMAT format_make_srgb( DXGI_FORMAT const in_format );
bool format_is_dxt( DXGI_FORMAT const in_format );

D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle_offset( D3D12_CPU_DESCRIPTOR_HANDLE const start, uptr const index );
D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle_offset( D3D12_CPU_DESCRIPTOR_HANDLE const start, uptr const index );
D3D12_CPU_DESCRIPTOR_HANDLE cbv_srv_uav_handle_offset( D3D12_CPU_DESCRIPTOR_HANDLE const start, uptr const index );
D3D12_CPU_DESCRIPTOR_HANDLE sampler_handle_offset( D3D12_CPU_DESCRIPTOR_HANDLE const start, uptr const index );

} // namespace render

#endif // #ifndef GUARD_RENDER_DX12_H_INCLUDED