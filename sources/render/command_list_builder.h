#ifndef GUARD_RENDER_COMMAND_LIST_BUILDER_H_INCLUDED
#define GUARD_RENDER_COMMAND_LIST_BUILDER_H_INCLUDED

#include <types.h>
#include "dx12.h"
#include "dx_command_list.h"
#include "pipeline_state.h"

namespace render {

class command_list_builder
{
public:
	command_list_builder( dx_command_list& in_cmd_list );
	~command_list_builder( );

	void bind_pipeline( graphics_ps const& in_ps );
	void bind_pipeline( compute_ps const& in_ps );

	// Valid for graphics pipeline.
	void bind_cbv( shader_type const in_shader_type, u32 const in_register, ID3D12Resource* const in_resource );
	void bind_srv( shader_type const in_shader_type, u32 const in_register, ID3D12Resource* const in_resource );
	void bind_uav( shader_type const in_shader_type, u32 const in_register, ID3D12Resource* const in_resource );

	// Valid for compute pipeline.
	void bind_cbv( u32 const in_register, ID3D12Resource* const in_resource );
	void bind_srv( u32 const in_register, ID3D12Resource* const in_resource );
	void bind_uav( u32 const in_register, ID3D12Resource* const in_resource );

	void set_viewport_and_scissors(
		math::u32x2 const& in_screen_dimensions,
		math::float2 const& in_depth_dimensions = math::float2( 0.0f, 1.0f )
	);

	inline operator dx_command_list( ) const { return m_cmd_list; }
	inline ID3D12GraphicsCommandList* operator->( ) const { return m_cmd_list; }

private:
	dx_command_list& m_cmd_list;
	graphics_ps const* m_gps = nullptr;
	compute_ps const* m_cps = nullptr;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_COMMAND_LIST_BUILDER_H_INCLUDED