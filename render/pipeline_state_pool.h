#ifndef __render_pipeline_state_pool_h_included_
#define __render_pipeline_state_pool_h_included_

#include <types.h>
#include "dx_include.h"
#include "pipeline_state.h"

namespace render {

template<u32 DS_max, u32 B_max, u32 R_max>
class pipeline_state_pool
{
public:
	depth_stencil_state create_depth_stencil( depth_stencil_state::cook const& in_cook );
	blend_state create_blend( blend_state::cook const& in_cook );
	rasterizer_state create_rasterizer( rasterizer_state::cook const& in_cook );

	void destroy( );

protected:
	struct A
	{
		depth_stencil_state			state;
		depth_stencil_state::cook	cook;
	} m_depth_stencil_data[DS_max];

	struct
	{
		blend_state			state;
		blend_state::cook	cook;
	} m_blend_data[B_max];
	
	struct
	{
		rasterizer_state::cook	cook;
		rasterizer_state		state;
	} m_rasterizer_data[R_max];
};

} // namespace render

#endif // #ifndef __render_pipeline_state_pool_h_included_