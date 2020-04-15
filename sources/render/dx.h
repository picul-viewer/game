#ifndef GUARD_RENDER_DX_H_INCLUDED
#define GUARD_RENDER_DX_H_INCLUDED

#include <types.h>
#include "dx12.h"

namespace render {

static const u32 max_frame_delay = 2;

class dx
{
public:
	static const DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const u32 swap_chain_buffer_count = max_frame_delay;

public:
	void create( );
	void destroy( );

	inline ID3D12Device* device( ) const { return m_device; }
	inline IDXGISwapChain3* swap_chain( ) const { return m_swap_chain; }
	inline ID3D12Resource* swap_chain_buffer( u32 const in_index ) const { ASSERT_CMP( in_index, <, 2 ); return m_swap_chain_buffers[in_index]; }
	inline ID3D12CommandQueue* queue_graphics( ) const { return m_queue_graphics; }
	inline ID3D12CommandQueue* queue_compute( ) const { return m_queue_compute; }
	inline ID3D12CommandQueue* queue_copy( ) const { return m_queue_copy; }
	inline u32 rtv_descriptor_size( ) const { return m_rtv_descriptor_size; }
	inline u32 dsv_descriptor_size( ) const { return m_dsv_descriptor_size; }
	inline u32 cbv_srv_uav_descriptor_size( ) const { return m_cbv_srv_uav_descriptor_size; }
	inline u32 sampler_descriptor_size( ) const { return m_sampler_descriptor_size; }
	
private:
	ID3D12Device* m_device;
	IDXGISwapChain3* m_swap_chain;
	ID3D12Resource* m_swap_chain_buffers[swap_chain_buffer_count];
	ID3D12CommandQueue* m_queue_graphics;
	ID3D12CommandQueue* m_queue_compute;
	ID3D12CommandQueue* m_queue_copy;
	u32 m_rtv_descriptor_size;
	u32 m_dsv_descriptor_size;
	u32 m_cbv_srv_uav_descriptor_size;
	u32 m_sampler_descriptor_size;

};


extern dx g_dx;

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_H_INCLUDED