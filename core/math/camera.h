#ifndef __core_camera_h_included_
#define __core_camera_h_included_

#include <types.h>
#include "matrix.h"

class camera
{
public:
	void set_view_projection( math::float4x4 const& in_view, float in_fov, float in_aspect, float in_znear, float in_zfar );
	void set_view_projection( math::float4x4 const& in_view, math::float4x4 in_projection );
	void set_projection( float in_fov, float in_aspect, float in_znear, float in_zfar );
	void set_projection( math::float4x4 const& in_projection );
	void set_view( math::float4x4 const& in_view );

	inline math::float4x4 get_view( ) const { return m_view; }
	inline math::float4x4 get_projection( ) const { return m_projection; }
	inline math::float4x4 get_view_projection( ) const { return m_view_projection; }
	inline math::float4x4 get_inv_view( ) const { return m_inv_view; }

protected:
	void mult_view_projection( );

	math::float4x4	m_view;
	math::float4x4	m_projection;
	math::float4x4	m_view_projection;
	math::float4x4	m_inv_view;
};

#endif // #ifndef __core_camera_h_included_