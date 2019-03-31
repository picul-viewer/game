#ifndef GUARD_RENDER_CAMERA_H_INCLUDED
#define GUARD_RENDER_CAMERA_H_INCLUDED

#include <types.h>
#include <math/math_sse.h>

namespace render {

class camera
{
public:
	void set_view( math::sse::matrix const& in_view );
	void set_perspective( float fov, float aspect, float near, float far );
	void update( );
	
	inline math::sse::matrix const& get_view( ) const { return m_view; }
	inline math::sse::matrix const& get_projection( ) const { return m_projection; }
	inline math::sse::matrix const& get_view_projection( ) const { return m_view_projection; }

	inline math::sse::matrix& get_view( ) { return m_view; }
	inline math::sse::matrix& get_projection( ) { return m_projection; }
	inline math::sse::matrix& get_view_projection( ) { return m_view_projection; }

protected:
	math::sse::matrix m_view;
	math::sse::matrix m_projection;
	math::sse::matrix m_view_projection;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_CAMERA_H_INCLUDED