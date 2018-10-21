#include "camera.h"
#include <math/math_3d.h>

namespace render {

void camera::set_view( math::sse::matrix const& in_view )
{
	m_view = in_view;
}

void camera::set_perspective( float fov, float aspect, float near, float far )
{
	mem_align(16) math::float4x4 proj = math::matrix_perspective( fov, aspect, near, far );
	m_projection.load( proj.data );
}

void camera::update( )
{
	// Calculate view-projection.
	m_view_projection = math::sse::combine_transform_and_perspective_projection( m_view, m_projection );
}

} // namespace render