#ifndef GUARD_RENDER_GPU_LINEAR_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_GPU_LINEAR_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include "dx12.h"

namespace render {

class gpu_linear_allocator
{
public:
	static u64 precompute_resources_offsets(
		D3D12_RESOURCE_ALLOCATION_INFO const* const in_allocation_infos,
		u32 const in_allocation_info_count,
		u64* const out_allocation_offsets
	);

	void create( );
	void destroy( );

	u64 allocate( D3D12_RESOURCE_ALLOCATION_INFO const& in_allocation_info );

private:
	u64 m_current_offset;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_LINEAR_ALLOCATOR_H_INCLUDED