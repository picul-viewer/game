#include "dx_descriptor_heap.h"
#include "dx.h"

namespace render {

void dx_descriptor_heap::create( D3D12_DESCRIPTOR_HEAP_TYPE const in_type, u32 const in_descriptor_count, bool const in_shader_visible )
{
	D3D12_DESCRIPTOR_HEAP_DESC desc;

	desc.Type = in_type;
	desc.NumDescriptors = in_descriptor_count;
	desc.Flags = in_shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	DX12_CHECK_RESULT( g_dx.device( )->CreateDescriptorHeap( &desc, IID_PPV_ARGS( &m_heap ) ) );
}

void dx_descriptor_heap::destroy( )
{
	s32 const ref_cnt = m_heap->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render