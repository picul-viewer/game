#include "barrier_builder.h"

namespace render {

barrier_builder::barrier_builder( dx_command_list const in_cmd_list ) :
	m_cmd_list( in_cmd_list )
{ }

barrier_builder::~barrier_builder( )
{
	m_cmd_list->ResourceBarrier( m_count, m_data );
}

void barrier_builder::transition( ID3D12Resource* const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource )
{
	ASSERT( m_count < max_barriers );
	D3D12_RESOURCE_BARRIER& desc = m_data[m_count++];
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Transition.pResource = in_resource;
	desc.Transition.Subresource = in_subresource;
	desc.Transition.StateBefore = in_before_state;
	desc.Transition.StateAfter = in_after_state;
}

void barrier_builder::transition_begin( ID3D12Resource* const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource )
{
	ASSERT( m_count < max_barriers );
	D3D12_RESOURCE_BARRIER& desc = m_data[m_count++];
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;
	desc.Transition.pResource = in_resource;
	desc.Transition.Subresource = in_subresource;
	desc.Transition.StateBefore = in_before_state;
	desc.Transition.StateAfter = in_after_state;
}

void barrier_builder::transition_end( ID3D12Resource* const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource )
{
	ASSERT( m_count < max_barriers );
	D3D12_RESOURCE_BARRIER& desc = m_data[m_count++];
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
	desc.Transition.pResource = in_resource;
	desc.Transition.Subresource = in_subresource;
	desc.Transition.StateBefore = in_before_state;
	desc.Transition.StateAfter = in_after_state;
}

void barrier_builder::aliasing( ID3D12Resource* const in_resource_before, ID3D12Resource* const in_resource_after )
{
	ASSERT( m_count < max_barriers );
	D3D12_RESOURCE_BARRIER& desc = m_data[m_count++];
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Aliasing.pResourceBefore = in_resource_before;
	desc.Aliasing.pResourceAfter = in_resource_after;
}

void barrier_builder::uav( ID3D12Resource* const in_resource )
{
	ASSERT( m_count < max_barriers );
	D3D12_RESOURCE_BARRIER& desc = m_data[m_count++];
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.UAV.pResource = in_resource;
}

} // namespace render