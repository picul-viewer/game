#ifndef __render_per_frame_constants_h_included_
#define __render_per_frame_constants_h_included_

#include <core/types.h>
#include <core/macros.h>
#include <core/math.h>

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

} // namespace render

#endif // #ifndef __render_per_frame_constants_h_included_