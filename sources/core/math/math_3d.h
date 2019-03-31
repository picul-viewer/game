#ifndef GUARD_CORE_MATH_3D_H_INCLUDED
#define GUARD_CORE_MATH_3D_H_INCLUDED

#include "vector.h"
#include "matrix.h"

namespace math {

static const float c_epsilon = 1e-7f;
static const float c_pi = 3.1415926536f;
static const float c_radians = c_pi / 180.0f;
static const float c_degrees = 180.0f / c_pi;

inline float degree_to_radian( float angle )
{
	return angle * c_radians;
}

inline float radian_to_degree( float angle )
{
	return angle * c_degrees;
}


// This functions define how to apply and combine transformations
// DIY, if you remember how

// Transform vector.
inline float4 modify_by_transform( float4 const& v, float4x4 const& m )
{
	return mul( m, v );
}

// This assumes that v.w = 1.0
inline float3 modify_position_by_transform( float3 const& v, float4x4 const& m )
{
	return mul( float3x3( m ), v ) + float3( m.a03, m.a13, m.a23 );
}

// Result is equal to: m1 is applied first, m2 is applied second.
inline float4x4 combine_transforms( float4x4 const& m1, float4x4 const& m2 )
{
	return mul( m2, m1 );
}

// Result is equal to: m1 is applied first, m2 is applied second.
inline float4x3 combine_transforms( float4x3 const& m1, float4x3 const& m2 )
{
	//return mul( m2, m1 );
	float4x3 result	= float4x3( mul( float3x3( m2 ), float3x3( m1 ) ) );
	
	result.a03		= m2.a00 * m1.a03 + m2.a01 + m1.a13 + m2.a02 * m1.a23 + m2.a03;
	result.a13		= m2.a10 * m1.a03 + m2.a11 + m1.a13 + m2.a12 * m1.a23 + m2.a13;
	result.a23		= m2.a20 * m1.a03 + m2.a21 + m1.a13 + m2.a22 * m1.a23 + m2.a23;

	return result;
}


inline float4x3 matrix_rotation_x( float angle )
{
	float c = cosf( angle ), s = sinf( angle );

	return float4x3( float3( 1.0f, 0.0f, 0.0f ),
					 float3( 0.0f, c, s ),
					 float3( 0.0f, -s, c ) );
}

inline float4x3 matrix_rotation_y( float angle )
{
	float c = cosf( angle ), s = sinf( angle );

	return float4x3( float3( c, 0.0f, -s ),
					 float3( 0.0f, 1.0f, 0.0f ),
					 float3( s, 0.0f, c ) );
}

inline float4x3 matrix_rotation_z( float angle )
{
	float c = cosf( angle ), s = sinf( angle );

	return float4x3( float3( c, s, 0.0f ),
					 float3( -s, c, 0.0f ),
					 float3( 0.0f, 0.0f, 1.0f ) );
}

inline float4x3 matrix_rotation_axis( float3 const& axis, float angle )
{
	float const c = cosf( angle );
	float const s = sinf( angle );

	float const xx = axis.x * axis.x;
	float const xy = axis.x * axis.y;
	float const xz = axis.x * axis.z;
	float const yy = axis.y * axis.y;
	float const yz = axis.y * axis.z;
	float const zz = axis.z * axis.z;

	float const sx = s * axis.x;
	float const sy = s * axis.y;
	float const sz = s * axis.z;

	float const inv_c = 1 - c;
	
	float const inv_c_xx = inv_c * xx;
	float const inv_c_yy = inv_c * yy;
	float const inv_c_zz = inv_c * zz;

	float const inv_c_xy = inv_c * xy;
	float const inv_c_xz = inv_c * xz;
	float const inv_c_yz = inv_c * yz;

	return float4x3( float3( c + inv_c_xx, inv_c_xy - sz, inv_c_xz + sy ),
					 float3( inv_c_xy + sz, c + inv_c_yy, inv_c_yz - sx ),
					 float3( inv_c_xz - sy, inv_c_yz + sx, c + inv_c_zz ) );
}

inline float4x3 matrix_scale( float2 const& s )
{
	return float4x3( float3( s.x, 0.0f, 0.0f ),
					 float3( 0.0f, s.y, 0.0f ),
					 float3( 0.0f, 0.0f, 1.0f ) );
}

inline float4x3 matrix_scale( float3 const& s )
{
	return float4x3( float4( s.x, 0.0f, 0.0f, 0.0f ),
					 float4( 0.0f, s.y, 0.0f, 0.0f ),
					 float4( 0.0f, 0.0f, s.z, 0.0f ) );
}

inline float3x3 matrix_translation( float2 const& t )
{
	return float3x3( float3( 1.0f, 0.0f, t.x ),
					 float3( 0.0f, 1.0f, t.y ),
					 float3( 0.0f, 0.0f, 1.0f ) );
}

inline float4x3 matrix_translation( float3 const& t )
{
	return float4x3( float4( 1.0f, 0.0f, 0.0f, t.x ),
					 float4( 0.0f, 1.0f, 0.0f, t.y ),
					 float4( 0.0f, 0.0f, 1.0f, t.z ) );
}

inline float4x4 matrix_transformation_inverse( float4x4 const& m )
{
	float4x4 result;

	result.a00		= m.a00;
	result.a01		= m.a10;
	result.a02		= m.a20;
	result.a03		= - m.a30 * m.a00 - m.a31 * m.a10 - m.a32 * m.a20;
	
	result.a10		= m.a01;
	result.a11		= m.a11;
	result.a12		= m.a21;
	result.a13		= - m.a30 * m.a01 - m.a31 * m.a11 - m.a32 * m.a21;

	result.a20		= m.a02;
	result.a21		= m.a12;
	result.a22		= m.a22;
	result.a23		= - m.a30 * m.a02 - m.a31 * m.a12 - m.a32 * m.a22;

	result.l3		= float4( 0.0f, 0.0f, 0.0f, 1.0f );

	return result;
}

inline float4x4 matrix_transformation_and_scale_inverse( float4x4 const& m )
{
	float4x4 result	= float4x4( inverse( float3x3( m ) ) );
	
	result.a03		= - m.a30 * result.a00 - m.a31 * result.a01 - m.a32 * result.a02;
	result.a13		= - m.a30 * result.a10 - m.a31 * result.a11 - m.a32 * result.a12;
	result.a23		= - m.a30 * result.a20 - m.a31 * result.a21 - m.a32 * result.a22;

	return result;
}

inline float4x4 matrix_orthographic( float left, float right, float bottom, float top, float znear, float zfar )
{
	const float
		tx = -( right + left ) / ( right - left ),
		ty = -( top + bottom ) / ( top - bottom ),
		tz = -( zfar + znear ) / ( zfar - znear );

	return float4x4( float4( 2.0f / ( right - left ), 0.0f, 0.0f, tx ),
					 float4( 0.0f, 2.0f / ( top - bottom ), 0.0f, ty ),
					 float4( 0.0f, 0.0f, -2.0f / ( zfar - znear ), tz ),
					 float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

// depth: znear - 1, zfar - 0
inline float4x4 matrix_perspective( float fov, float aspect, float znear, float zfar )
{
	float const
		f = 1.0f / tan( 0.5f * fov ),
		A =  zfar / ( zfar - znear ),
		B = - A * znear;

	return float4x4( float4( f / aspect, 0.0f, 0.0f, 0.0f ),
					 float4( 0.0f, f, 0.0f, 0.0f ),
					 float4( 0.0f, 0.0f, A, B ),
					 float4( 0.0f, 0.0f, 1.0f, 0.0f ) );
}

inline float4x4 combine_transform_and_perspective_projection( float4x4 const& view, float4x4 const& perspective_projection )
{
	/*
	return combine_transforms( view, perspective_projection );
	*/

	return float4x4(
		view.l0 * perspective_projection.a00,
		view.l1 * perspective_projection.a11,
		view.l2 * perspective_projection.a22 + view.l3 * perspective_projection.a23,
		view.l2
	);
}

inline float4x4 matrix_look_at_direction( float3 const& position, float3 const& direction, float3 const& up )
{
	float3 const& f = direction;
	float3 const& s = normalize( cross( up, f ) );
	float3 const& u = normalize( cross( f, s ) );

	/*
	return combine_transforms(
	matrix_translation( -position ),
	float4x4( float3x3( s, u, f ) )
	);
	*/

	return float4x4( float4( s.x, s.y, s.z, dot( -position, s ) ),
		float4( u.x, u.y, u.z, dot( -position, u ) ),
		float4( f.x, f.y, f.z, dot( -position, f ) ),
		float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

inline float4x4 matrix_look_at( float3 const& position, float3 const& center, float3 const& up )
{
	return matrix_look_at_direction( position, normalize( center - position ), up );
}

} // namespace math

#endif // #ifndef GUARD_CORE_MATH_3D_H_INCLUDED