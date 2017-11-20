#ifndef __render_per_material_constant_buffer_h_included_
#define __render_per_material_constant_buffer_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

#include "constant_buffer.h"

namespace render {

mem_align(16)
struct miscellaneous_constants
{
	math::float4	m_data0;
	math::float4	m_data1;
	math::float4	m_data2;
	math::float4	m_data3;
};

typedef constant_buffer<miscellaneous_constants> miscellaneous_constant_buffer;

} // namespace render

#endif // #ifndef __render_per_material_constant_buffer_h_included_