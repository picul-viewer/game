#include "dx_command_allocator.h"
#include "dx.h"

namespace render {

void dx_command_allocator::create( D3D12_COMMAND_LIST_TYPE const in_type )
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateCommandAllocator( in_type, IID_PPV_ARGS( &m_allocator ) ) );
}

void dx_command_allocator::destroy( )
{
	s32 const ref_cnt = m_allocator->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

} // namespace render