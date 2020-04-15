#include "dx_command_list.h"
#include "dx.h"

namespace render {

void dx_command_list::create(
	D3D12_COMMAND_LIST_TYPE const in_type,
	dx_command_allocator const& in_allocator,
	ID3D12PipelineState* const in_initial_state
)
{
	DX12_CHECK_RESULT( g_dx.device( )->CreateCommandList( 0, in_type, in_allocator, in_initial_state, IID_PPV_ARGS( &m_list ) ) );
}

void dx_command_list::destroy( )
{
	s32 const ref_cnt = m_list->Release( );
	ASSERT_CMP( ref_cnt, ==, 0 );
}

void dx_command_list::cmd_set_viewport_and_scissors(
	math::u32x2 const& in_screen_dimensions,
	math::float2 const& in_depth_dimensions
)
{
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)in_screen_dimensions.x;
	viewport.Height = (float)in_screen_dimensions.y;
	viewport.MinDepth = in_depth_dimensions.x;
	viewport.MaxDepth = in_depth_dimensions.y;
	m_list->RSSetViewports( 1, &viewport );

	D3D12_RECT scissor_rect;
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = in_screen_dimensions.x;
	scissor_rect.bottom = in_screen_dimensions.y;
	m_list->RSSetScissorRects( 1, &scissor_rect );
}

} // namespace render