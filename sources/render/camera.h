#ifndef GUARD_RENDER_CAMERA_H_INCLUDED
#define GUARD_RENDER_CAMERA_H_INCLUDED

#include <types.h>
#include <math/math_sse.h>
#include "gpu_structures.h"

namespace render {

class camera
{
public:
	void set_view( math::float4x4 const& in_view );
	void set_perspective( float in_fov, float in_aspect, float in_near, float in_far );
	void update( );

	void fill_constant_buffer( gpu::constant_buffer& in_buffer );

	inline math::float4x4& get_view( ) { return m_view; }
	inline math::float4x4& get_projection( ) { return m_projection; }
	inline math::float4x4& get_view_projection( ) { return m_view_projection; }

protected:
	math::float4x4 m_view;
	math::float4x4 m_projection;
	math::float4x4 m_view_projection;
	float m_near;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_CAMERA_H_INCLUDED