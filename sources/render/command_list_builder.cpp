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

	ID3D12DescriptorHeap* const heap = g_resources.descriptor_heap( );
	m_cmd_list->SetDescriptorHeaps( 1, &heap );

	u32 const current_table = g_resources.allocate_descriptors( in_ps.get_table_size( ) );
	m_current_table = current_table;

	m_gps->set_descriptor_tables( m_cmd_list, current_table );
}

void command_list_builder::bind_pipeline( compute_ps const& in_ps )
{
	ASSERT( !m_gps || !m_cps );
	m_gps = nullptr;
	m_cps = &in_ps;

	m_cmd_list->SetPipelineState( m_cps->ps( ) );
	m_cmd_list->SetComputeRootSignature( m_cps->rs( ) );

	ID3D12DescriptorHeap* const heap = g_resources.descriptor_heap( );
	m_cmd_list->SetDescriptorHeaps( 1, &heap );

	u32 const current_table = g_resources.allocate_descriptors( in_ps.get_table_size( ) );
	m_current_table = current_table;

	m_cps->set_descriptor_tables( m_cmd_list, current_table );
}

void command_list_builder::bind_root_cbv( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	u32 const index = m_gps->get_cbv_root_index( in_shader_type, in_register );
	m_cmd_list->SetComputeRootConstantBufferView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_root_srv( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	u32 const index = m_gps->get_srv_root_index( in_shader_type, in_register );
	m_cmd_list->SetComputeRootShaderResourceView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_root_uav( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_gps );
	u32 const index = m_gps->get_uav_root_index( in_shader_type, in_register );
	m_cmd_list->SetComputeRootUnorderedAccessView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_table_cbv( shader_type const in_shader_type, u32 const in_register, dx_cbv_cook const& in_cook )
{
	ASSERT( m_gps );
	u32 const offset = m_gps->get_cbv_table_offset( in_shader_type, in_register );
	g_resources.create_cbv( m_current_table + offset, in_cook );
}

void command_list_builder::bind_table_srv( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource, dx_srv_cook const& in_cook )
{
	ASSERT( m_gps );
	u32 const offset = m_gps->get_srv_table_offset( in_shader_type, in_register );
	g_resources.create_srv( m_current_table + offset, in_resource, in_cook );
}

void command_list_builder::bind_table_uav( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource, dx_uav_cook const& in_cook )
{
	ASSERT( m_gps );
	u32 const offset = m_gps->get_uav_table_offset( in_shader_type, in_register );
	g_resources.create_uav( m_current_table + offset, in_resource, in_cook );
}

void command_list_builder::bind_table_uav( shader_type const in_shader_type, u32 const in_register, dx_resource const in_resource, dx_resource const in_counter_resource, dx_uav_cook const& in_cook )
{
	ASSERT( m_gps );
	u32 const offset = m_gps->get_uav_table_offset( in_shader_type, in_register );
	g_resources.create_uav( m_current_table + offset, in_resource, in_counter_resource, in_cook );
}

void command_list_builder::bind_root_cbv( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	u32 const index = m_cps->get_cbv_root_index( in_register );
	m_cmd_list->SetComputeRootConstantBufferView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_root_srv( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	u32 const index = m_cps->get_srv_root_index( in_register );
	m_cmd_list->SetComputeRootShaderResourceView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_root_uav( u32 const in_register, dx_resource const in_resource )
{
	ASSERT( m_cps );
	u32 const index = m_cps->get_uav_root_index( in_register );
	m_cmd_list->SetComputeRootUnorderedAccessView( index, in_resource->GetGPUVirtualAddress( ) );
}

void command_list_builder::bind_table_cbv( u32 const in_register, dx_cbv_cook const& in_cook )
{
	ASSERT( m_cps );
	u32 const offset = m_cps->get_cbv_table_offset( in_register );
	g_resources.create_cbv( m_current_table + offset, in_cook );
}

void command_list_builder::bind_table_srv( u32 const in_register, dx_resource const in_resource, dx_srv_cook const& in_cook )
{
	ASSERT( m_cps );
	u32 const offset = m_cps->get_srv_table_offset( in_register );
	g_resources.create_srv( m_current_table + offset, in_resource, in_cook );
}

void command_list_builder::bind_table_uav( u32 const in_register, dx_resource const in_resource, dx_uav_cook const& in_cook )
{
	ASSERT( m_cps );
	u32 const offset = m_cps->get_uav_table_offset( in_register );
	g_resources.create_uav( m_current_table + offset, in_resource, in_cook );
}

void command_list_builder::bind_table_uav( u32 const in_register, dx_resource const in_resource, dx_resource const in_counter_resource, dx_uav_cook const& in_cook )
{
	ASSERT( m_cps );
	u32 const offset = m_cps->get_uav_table_offset( in_register );
	g_resources.create_uav( m_current_table + offset, in_resource, in_counter_resource, in_cook );
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