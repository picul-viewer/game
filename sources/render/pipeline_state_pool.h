#ifndef __render_pipeline_state_pool_h_included_
#define __render_pipeline_state_pool_h_included_

#include <types.h>
#include "pipeline_state.h"

namespace render {

namespace __render_pipeline_state_pool_detail {

struct dss_data
{
	typedef depth_stencil_state			data_type;
	typedef depth_stencil_state::cook	cook_type;

	data_type	state;
	cook_type	cook;
};

struct bs_data
{
	typedef blend_state			data_type;
	typedef blend_state::cook	cook_type;

	data_type	state;
	cook_type	cook;
};

struct rs_data
{
	typedef rasterizer_state		data_type;
	typedef rasterizer_state::cook	cook_type;
		
	data_type	state;
	cook_type	cook;
};

} // namespace __render_pipeline_state_pool_detail

template<u32 DS_max, u32 B_max, u32 R_max>
class pipeline_state_pool
{
public:
	depth_stencil_state create_depth_stencil( depth_stencil_state::cook const& in_cook );
	blend_state create_blend( blend_state::cook const& in_cook );
	rasterizer_state create_rasterizer( rasterizer_state::cook const& in_cook );

	void destroy( );

protected:
	__render_pipeline_state_pool_detail::dss_data m_depth_stencil_data[DS_max];
	__render_pipeline_state_pool_detail::bs_data m_blend_data[B_max];
	__render_pipeline_state_pool_detail::rs_data m_rasterizer_data[R_max];
};

} // namespace render

#include "pipeline_state_pool_inline.h"

#endif // #ifndef __render_pipeline_state_pool_h_included_