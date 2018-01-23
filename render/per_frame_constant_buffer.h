#ifndef __render_per_frame_constant_buffer_h_included_
#define __render_per_frame_constant_buffer_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

#include "constant_buffer.h"

namespace render {

mem_align(16)
struct per_frame_constants
{
	math::float2	m_resolution;
	math::float2	m_inversed_resolution;
	float			m_current_time;
	float			m_delta_time;
	math::float2	__unused;
};

typedef constant_buffer<per_frame_constants> per_frame_constant_buffer;

} // namespace render

#endif // #ifndef __render_per_frame_constant_buffer_h_included_