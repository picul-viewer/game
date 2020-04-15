#include "dx_heap.h"
#include "dx.h"

namespace render {

void dx_heap::create(
	u64 const in_size,
	D3D12_HEAP_TYPE const in_type,
	bool const in_allow_textures,
	bool const in_allow_targets,
	bool const in_allow_buffers
)
{
	D3D12_HEAP_DESC desc;

	desc.SizeInBytes = in_size;
	desc.Properties.Type = in_type;
	desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	desc.Properties.CreationNodeMask = 0;
	desc.Properties.VisibleNodeMask = 0;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	desc.Flags =
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES |
		( in_allow_textures ? D3D12_HEAP_FLAG_NONE : D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES ) |
		( in_allow_targets ? D3D12_HEAP_FLAG_NONE : D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES ) |
		( in_allow_buffers ? D3D12_HEAP_FLAG_NONE : D3D12_HEAP_FLAG_DENY_BUFFERS );

	DX12_CHECK_RESULT( g_dx.device( )->CreateHeap( &desc, IID_PPV_ARGS( &m_heap ) ) );
}

void dx_heap::destroy( )
{
	s32 const ref_cnt = m_heap->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render