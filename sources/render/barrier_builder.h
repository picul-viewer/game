#ifndef GUARD_RENDER_BARRIER_BUILDER_H_INCLUDED
#define GUARD_RENDER_BARRIER_BUILDER_H_INCLUDED

#include <types.h>
#include "dx12.h"
#include "dx_command_list.h"
#include "dx_resource.h"

namespace render {

class barrier_builder
{
public:
	barrier_builder( dx_command_list const in_cmd_list );
	~barrier_builder( );

	void transition( dx_resource const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource = 0 );
	void transition_begin( dx_resource const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource = 0 );
	void transition_end( dx_resource const in_resource, D3D12_RESOURCE_STATES const in_before_state, D3D12_RESOURCE_STATES const in_after_state, u32 const in_subresource = 0 );

	void aliasing( dx_resource const in_resource_before, dx_resource const in_resource_after );

	void uav( dx_resource const in_resource );

private:
	enum { max_barriers = 16 };

private:
	D3D12_RESOURCE_BARRIER m_data[max_barriers];
	dx_command_list const m_cmd_list;
	u32 m_count = 0;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_BARRIER_BUILDER_H_INCLUDED