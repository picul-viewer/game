#include "command_list_builder.h"
#include "resources.h"

namespace render {

command_list_builder::command_list_builder( dx_command_list& in_cmd_list ) :
	m_cmd_list( in_cmd_list )
{ }

command_list_builder::~command_list_builder( )
{
	DX12_CHECK_RESULT( m_cmd_list->Close( ) );
}

void command_list_builder::bind_pipeline( graphics_ps const& in_ps )
{
	ASSERT( !m_gps || !m_cps );
	m_gps = &in_ps;
	m_cps = nullptr;

	m_cmd_list->SetPipelineState( m_gps->ps( ) );
	m_cmd_list->SetGraphicsRootSignature( m_gps->rs( ) );

	ID3D12DescriptorHeap* const heap = g_resources.srv_uav_heap( );
	m_cmd_list->SetDescriptorHeaps( 1, &heap );

	m_gps->set_descriptor_table( m_cmd_list, g_resources.srv_uav_heap( )->GetGPUDescriptorHandleForHeapStart( ) );
}

void command_list_builder::bind_pipeline( compute_ps const& in_ps )
{
	ASSERT( !m_gps || !m_cps );
	m_gps = nullptr;
	m_cps = &in_ps;

	m_cmd_list->SetPipelineState( m_cps->ps( ) );
	m_cmd_list->SetComputeRootSignature( m_cps->rs( ) );

	ID3D12DescriptorHeap* const heap = g_resources.srv_uav_heap( );
	m_cmd_list->SetDescriptorHeaps( 1, &heap );

	m_cps->set_descriptor_table( m_cmd_list, g_resources.srv_uav_heap( )->GetGPUDescriptorHandleForHeapStart( ) );
}

void command_list_builder::bind_cbv( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	m_gps->bind_cbv( in_shader_type, m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_srv( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	m_gps->bind_srv( in_shader_type, m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_uav( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	m_gps->bind_uav( in_shader_type, m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_cbv( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	m_cps->bind_cbv( m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_srv( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	m_cps->bind_srv( m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_uav( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	m_cps->bind_uav( m_cmd_list, in_register, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::set_viewport_and_scissors(
	math::u32x2 const& in_screen_dimensions,
	math::float2 const& in_depth_dimensions
)
{
	ASSERT( m_gps );

	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)in_screen_dimensions.x;
	viewport.Height = (float)in_screen_dimensions.y;
	viewport.MinDepth = in_depth_dimensions.x;
	viewport.MaxDepth = in_depth_dimensions.y;
	m_cmd_list->RSSetViewports( 1, &viewport );

	D3D12_RECT scissor_rect;
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = in_screen_dimensions.x;
	scissor_rect.bottom = in_screen_dimensions.y;
	m_cmd_list->RSSetScissorRects( 1, &scissor_rect );
}

} // namespace render