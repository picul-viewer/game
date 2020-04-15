#ifndef GUARD_RENDER_DX_COMMAND_LIST_H_INCLUDED
#define GUARD_RENDER_DX_COMMAND_LIST_H_INCLUDED

#include <types.h>
#include <math/vector.h>
#include "dx12.h"
#include "dx_command_allocator.h"
#include "dx_pipeline_state.h"

namespace render {

class dx_command_list
{
public:
	void create(
		D3D12_COMMAND_LIST_TYPE const in_type,
		dx_command_allocator const& in_allocator,
		ID3D12PipelineState* const in_initial_state
	);

	void destroy( );

	inline void set( ID3D12GraphicsCommandList* in_list ) { m_list = in_list; }

	inline operator ID3D12GraphicsCommandList*( ) const { return m_list; }
	inline ID3D12GraphicsCommandList* operator->( ) const { return m_list; }


	void cmd_set_viewport_and_scissors(
		math::u32x2 const& in_screen_dimensions,
		math::float2 const& in_depth_dimensions = math::float2( 0.0f, 1.0f )
	);

private:
	ID3D12GraphicsCommandList* m_list;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_DX_COMMAND_LIST_H_INCLUDED