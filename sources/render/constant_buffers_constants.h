#ifndef GUARD_RENDER_CONSTANT_BUFFERS_CONSTANTS_H_INCLUDED
#define GUARD_RENDER_CONSTANT_BUFFERS_CONSTANTS_H_INCLUDED

#include <types.h>
#include <math/vector.h>

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

mem_align(16)
struct per_instance_constants
{
	math::float4	m_data0;
	math::float4	m_data1;
	math::float4	m_data2;
	math::float4	m_data3;
	math::float4	m_data4;
	math::float4	m_data5;
	math::float4	m_data6;
	math::float4	m_data7;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_CONSTANT_BUFFERS_CONSTANTS_H_INCLUDED