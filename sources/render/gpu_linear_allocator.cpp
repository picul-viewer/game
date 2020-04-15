#include "gpu_linear_allocator.h"

#include <math/math_common.h>

#include "dx.h"

namespace render {

u64 gpu_linear_allocator::precompute_resources_offsets(
	D3D12_RESOURCE_ALLOCATION_INFO const* const in_allocation_infos,
	u32 const in_allocation_info_count,
	u64* const out_allocation_offsets
)
{
	u64 current_offset = 0;

	for (u32 i = 0; i < in_allocation_info_count; ++i)
	{
		current_offset = math::align_up( current_offset, in_allocation_infos[i].Alignment );

		out_allocation_offsets[i] = current_offset;

		current_offset += in_allocation_infos[i].SizeInBytes;
	}

	return current_offset;
}

void gpu_linear_allocator::create( )
{
	m_current_offset = 0;
}

void gpu_linear_allocator::destroy( )
{ }

u64 gpu_linear_allocator::allocate( D3D12_RESOURCE_ALLOCATION_INFO const& in_allocation_info )
{
	m_current_offset = math::align_up( m_current_offset, in_allocation_info.Alignment );
	u64 const result = m_current_offset;
	m_current_offset += in_allocation_info.SizeInBytes;
	return result;
}

} // namespace render