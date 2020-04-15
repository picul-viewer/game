#include "dx_fence.h"
#include "dx.h"

namespace render {

void dx_fence::create( u64 const in_initial_value )
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateFence( in_initial_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_fence ) ) );
}

void dx_fence::destroy( )
{
	s32 const ref_cnt = m_fence->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render