#include "dx_query_heap.h"
#include "dx.h"

namespace render {

void dx_query_heap::create( u32 const in_count, D3D12_QUERY_HEAP_TYPE const in_type )
{
	D3D12_QUERY_HEAP_DESC desc;

	desc.Count = in_count;
	desc.Type = in_type;
	desc.NodeMask = 0;

	DX12_CHECK_RESULT( g_dx.device( )->CreateQueryHeap( &desc, IID_PPV_ARGS( &m_heap ) ) );
}

void dx_query_heap::destroy( )
{
	s32 const ref_cnt = m_heap->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render