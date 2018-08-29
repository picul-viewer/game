#ifndef __core_quaternion_h_included_
#define __core_quaternion_h_included_

#include <types.h>
#include "vector.h"
#include "matrix.h"

namespace math {

struct quat : float4
{
	inline quat( ) = default;
	inline quat( float v ) : float4( v ) { }
	inline quat( float x, float y, float z, float w ) : float4( x, y, z, w ) { }
};

inline quat inverse( quat const& q )
{
	return quat( -q.x, -q.y, -q.z, q.w );
}

inline float3 const& mul( quat const& q, float3 const& v )
{
	//return float3(q * quat(v) * inverse(q));
	const float
		xx = q.x * q.x,
		xy = q.x * q.y,
		xz = q.x * q.z,
		yy = q.y * q.y,
		yz = q.y * q.z,
		zz = q.z * q.z,
		xw = q.x * q.w,
		yw = q.y * q.w, 
		zw = q.z * q.w, 
		ww = q.w * q.w;

	return float3(
		( xx + ww - zz - yy ) * v.x + 2 * ( xy - zw ) * v.y + 2 * ( xz + yw ) * v.z,
		2 * ( xy + zw ) * v.x + ( yy - zz + ww - xx ) * v.y + 2 * ( yz - xw ) * v.z,
		2 * ( xz - yw ) * v.x + 2 * ( yz + xw ) * v.y + ( zz - yy - xx + ww ) * v.z );
}

inline float3x3 const& quat_to_mat( quat const& q )
{
	const float
		xx = q.x * q.x,
		xy = q.x * q.y,
		xz = q.x * q.z,
		xw = q.x * q.w,
		yy = q.y * q.y,
		yz = q.y * q.z,
		yw = q.y * q.w,
		zz = q.z * q.z,
		zw = q.z * q.w;

	return float3x3( float3( 1 - 2 * ( yy + zz ), 2 * ( xy - zw ), 2 * ( xz + yw ) ),
					 float3( 2 * ( xy + zw ), 1 - 2 * ( xx + zz ), 2 * ( yz - xw ) ),
					 float3( 2 * ( xz - yw ), 2 * ( yz + xw ), 1 - 2 * ( xx + yy ) ) );
}

} // namespace math

#endif // #ifndef __core_quaternion_h_included_