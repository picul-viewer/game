#include "camera.h"

#include "math_3d.h"

void camera::mult_view_projection( )
{
	m_view_projection.a00	= m_projection.a00 * m_view.a00;
	m_view_projection.a01	= m_projection.a11 * m_view.a01;
	m_view_projection.a02	= m_projection.a22 * m_view.a02 + m_view.a03;
	m_view_projection.a03	= m_view.a02;

	m_view_projection.a10	= m_projection.a00 * m_view.a10;
	m_view_projection.a11	= m_projection.a11 * m_view.a11;
	m_view_projection.a12	= m_projection.a22 * m_view.a12 + m_view.a13;
	m_view_projection.a13	= m_view.a12;

	m_view_projection.a20	= m_projection.a00 * m_view.a20;
	m_view_projection.a21	= m_projection.a11 * m_view.a21;
	m_view_projection.a22	= m_projection.a22 * m_view.a22 + m_view.a23;
	m_view_projection.a23	= m_view.a22;

	m_view_projection.a30	= m_projection.a00 * m_view.a30;
	m_view_projection.a31	= m_projection.a11 * m_view.a31;
	m_view_projection.a32	= m_projection.a22 * m_view.a32 + m_view.a33;
	m_view_projection.a33	= m_view.a32;
}

void camera::set_view_projection( math::float4x4 const& in_view, float in_fov, float in_aspect, float in_znear, float in_zfar )
{
	m_view			= in_view;
	m_inv_view		= math::matrix_transformation_inverse( m_view );
	m_projection	= math::matrix_perspective( in_fov, in_aspect, in_znear, in_zfar );
	mult_view_projection( );
}

void camera::set_view_projection( math::float4x4 const& in_view, math::float4x4 in_projection )
{
	m_view			= in_view;
	m_inv_view		= math::matrix_transformation_inverse( m_view );
	m_projection	= in_projection;
	mult_view_projection( );
}

void camera::set_projection( float in_fov, float in_aspect, float in_znear, float in_zfar )
{
	m_projection	= math::matrix_perspective( in_fov, in_aspect, in_znear, in_zfar );
	mult_view_projection( );
}

void camera::set_projection( math::float4x4 const& in_projection )
{
	m_projection	= in_projection;
	mult_view_projection( );
}

void camera::set_view( math::float4x4 const& in_view )
{
	m_view			= in_view;
	m_inv_view		= math::matrix_transformation_inverse( m_view );
	mult_view_projection( );
}