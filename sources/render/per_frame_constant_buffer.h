#ifndef GUARD_RENDER_PER_FRAME_CONSTANT_BUFFER_H_INCLUDED
#define GUARD_RENDER_PER_FRAME_CONSTANT_BUFFER_H_INCLUDED

#include <types.h>
#include <macros.h>

#include <math/vector.h>
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

#endif // #ifndef GUARD_RENDER_PER_FRAME_CONSTANT_BUFFER_H_INCLUDED