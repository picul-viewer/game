#ifndef __render_per_instance_constant_buffer_h_included_
#define __render_per_instance_constant_buffer_h_included_

#include <types.h>
#include <macros.h>

#include <math/matrix.h>
#include "dx_include.h"

#include "constant_buffer.h"

namespace render {

mem_align(16)
struct per_instance_constants
{
	math::float4x4	m_WVP;
};

typedef constant_buffer<per_instance_constants> per_instance_constant_buffer;

} // namespace render

#endif // #ifndef __render_per_instance_constant_buffer_h_included_