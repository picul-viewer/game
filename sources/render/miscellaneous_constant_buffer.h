#ifndef GUARD_RENDER_PER_MATERIAL_CONSTANT_BUFFER_H_INCLUDED
#define GUARD_RENDER_PER_MATERIAL_CONSTANT_BUFFER_H_INCLUDED

#include <types.h>
#include <macros.h>

#include <math/vector.h>

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

#endif // #ifndef GUARD_RENDER_PER_MATERIAL_CONSTANT_BUFFER_H_INCLUDED