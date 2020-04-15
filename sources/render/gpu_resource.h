#ifndef GUARD_RENDER_GPU_RESOURCE_H_INCLUDED
#define GUARD_RENDER_GPU_RESOURCE_H_INCLUDED

#include <types.h>
#include <math/vector.h>

#include "dx12.h"
#include "gpu_heap.h"

namespace render {

class gpu_resource
{
public:
	struct cook
	{
		void set_buffer(
			u64 const in_size,
			bool const in_allow_rtv,
			bool const in_allow_srv,
			bool const in_allow_uav
		);

		void set_texture_1d(
			DXGI_FORMAT const in_format,
			u64 const in_width,
			u32 const in_array_size,
			u32 const in_mip_levels,
			bool const in_allow_rtv,
			bool const in_allow_dsv,
			bool const in_allow_srv,
			bool const in_allow_uav
		);

		void set_texture_2d(
			DXGI_FORMAT const in_format,
			u64 const in_width,
			u32 const in_height,
			u32 const in_array_size,
			u32 const in_mip_levels,
			bool const in_allow_rtv,
			bool const in_allow_dsv,
			bool const in_allow_srv,
			bool const in_allow_uav
		);

		void set_texture_3d(
			DXGI_FORMAT const in_format,
			u64 const in_width,
			u32 const in_height,
			u32 const in_depth,
			u32 const in_mip_levels,
			bool const in_allow_rtv,
			bool const in_allow_dsv,
			bool const in_allow_srv,
			bool const in_allow_uav
		);

		D3D12_RESOURCE_ALLOCATION_INFO get_allocation_info( ) const;

		void set_resource_memory(
			ID3D12Heap* const in_heap,
			u64 const in_heap_offset
		);

		void set_initial_state(
			D3D12_RESOURCE_STATES const in_state
		);

		void set_color_clear_value(
			math::float4 const& in_color
		);

		void set_depth_clear_value(
			float const in_depth,
			u8 const in_stencil
		);

		D3D12_RESOURCE_DESC desc;
		ID3D12Heap* heap;
		u64 heap_offset;
		D3D12_RESOURCE_STATES initial_state;
		D3D12_CLEAR_VALUE clear_value;
	};

	void create( cook const& in_cook );
	void destroy( );

	inline ID3D12Resource* resource( ) const { return m_resource; }

private:
	ID3D12Resource* m_resource;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_RESOURCE_H_INCLUDED