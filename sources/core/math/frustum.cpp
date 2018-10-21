#include "frustum.h"

namespace math {

frustum::frustum( float4x4 const& m )
{
	set_from_matrix( m );
}

void frustum::set_from_matrix( float4x4 const& m )
{
	// All planes will look inside the frustum.

	planes[left_plane].vector	= float4(  m.a00 + m.a30,  m.a01 + m.a31,  m.a02 + m.a32,  m.a03 + m.a33 );
	planes[right_plane].vector	= float4( -m.a00 + m.a30, -m.a01 + m.a31, -m.a02 + m.a32, -m.a03 + m.a33 );

	planes[bottom_plane].vector	= float4(  m.a10 + m.a30,  m.a11 + m.a31,  m.a12 + m.a32,  m.a13 + m.a33 );
	planes[top_plane].vector	= float4( -m.a10 + m.a30, -m.a11 + m.a31, -m.a12 + m.a32, -m.a13 + m.a33 );

	planes[near_plane].vector	= float4(  m.a20,		   m.a21,		   m.a22,		   m.a23		 );
	planes[far_plane].vector	= float4( -m.a20 + m.a30, -m.a21 + m.a31, -m.a22 + m.a32, -m.a23 + m.a33 );

	// Should normalize planes, this means dividing plane vector by normal's
	// length, but it's not needed, see plane/aabb test implementation.
}

// true, if AABB is fully inside the frustum
bool frustum::plane::test_aabb_inside( aabb const& box ) const
{
	float3 const min_v( ( n.x > 0 ) ? box.m_min.x : box.m_max.x,
						( n.y > 0 ) ? box.m_min.y : box.m_max.y,
						( n.z > 0 ) ? box.m_min.z : box.m_max.z );

	float const value = dot( min_v, n ) + d;

	return value >= 0.0f;
}

// true, if AABB is fully outside the frustum
bool frustum::plane::test_aabb_outside( aabb const& box ) const
{
	float3 const max_v( ( n.x < 0 ) ? box.m_min.x : box.m_max.x,
						( n.y < 0 ) ? box.m_min.y : box.m_max.y,
						( n.z < 0 ) ? box.m_min.z : box.m_max.z );

	float const value = dot( max_v, n ) + d;

	return value < 0.0f;
}

intersection frustum::plane::test_aabb( aabb const& box ) const
{
	if ( test_aabb_outside( box ) )
		return outside;

	if ( test_aabb_inside( box ) )
		return inside;

	return intersect;
}

// true, if AABB is fully inside the frustum
bool frustum::test_aabb_inside( aabb const& box ) const
{
	for ( u32 i = 0; i < plane_count; ++i )
	{
		if ( !planes[i].test_aabb_inside( box ) )
			return false;
	}

	return true;
}

// true, if AABB is fully outside the frustum
bool frustum::test_aabb_outside( aabb const& box ) const
{
	for ( u32 i = 0; i < plane_count; ++i )
	{
		if ( planes[i].test_aabb_outside( box ) )
			return true;
	}

	return false;
}

intersection frustum::test_aabb( aabb const& box ) const
{
	for ( u32 i = 0; i < plane_count; ++i )
	{
		if ( planes[i].test_aabb_outside( box ) )
			return outside;
	}

	for ( u32 i = 0; i < plane_count; ++i )
	{
		if ( !planes[i].test_aabb_inside( box ) )
			return intersect;
	}

	return inside;
}


frustum_aligned::frustum_aligned( sse::matrix const& m )
{
	set_from_matrix( m );
}

void frustum_aligned::set_from_matrix( sse::matrix const& m )
{
	__m128 m0 = m[0];
	__m128 m1 = m[1];
	__m128 m2 = m[2];
	__m128 m3 = m[3];

	_MM_TRANSPOSE4_PS( m0, m1, m2, m3 );

	__m128 const mask = _mm_castsi128_ps( _mm_setr_epi32( 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x0 ) );

	planes[0] = _mm_add_ps( m0, _mm_and_ps( _mm_shuffle_ps( m0, m0, 0xFF ), mask ) );
	planes[1] = _mm_add_ps( m1, _mm_and_ps( _mm_shuffle_ps( m1, m1, 0xFF ), mask ) );
	planes[2] = _mm_add_ps( m2, _mm_and_ps( _mm_shuffle_ps( m2, m2, 0xFF ), mask ) );
	planes[3] = _mm_add_ps( m3, _mm_and_ps( _mm_shuffle_ps( m3, m3, 0xFF ), mask ) );
	
	planes[4] = _mm_sub_ps( _mm_shuffle_ps( m0, m0, 0xFF ), m0 );
	planes[5] = _mm_sub_ps( _mm_shuffle_ps( m1, m1, 0xFF ), m1 );
	planes[6] = _mm_sub_ps( _mm_shuffle_ps( m2, m2, 0xFF ), m2 );
	planes[7] = _mm_sub_ps( _mm_shuffle_ps( m3, m3, 0xFF ), m3 );
}

// true, if AABB is fully inside the frustum
bool frustum_aligned::test_aabb_inside( aabb_aligned const& box ) const
{
	__m128 const aabb_min = box.m_min;
	__m128 const aabb_max = box.m_max;

	__m128 const aabb_min_x = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_min_y = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_min_z = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	
	__m128 const aabb_max_x = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_max_y = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_max_z = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) );

	__m128 const planes0_x = planes[0];
	__m128 const planes0_y = planes[1];
	__m128 const planes0_z = planes[2];
	__m128 const planes0_w = planes[3];
	__m128 const planes1_x = planes[4];
	__m128 const planes1_y = planes[5];
	__m128 const planes1_z = planes[6];
	__m128 const planes1_w = planes[7];
	
	__m128 const aabb_min_p0_x = _mm_mul_ps( planes0_x, aabb_min_x );
	__m128 const aabb_min_p0_y = _mm_mul_ps( planes0_y, aabb_min_y );
	__m128 const aabb_min_p0_z = _mm_mul_ps( planes0_z, aabb_min_z );

	__m128 const aabb_max_p0_x = _mm_mul_ps( planes0_x, aabb_max_x );
	__m128 const aabb_max_p0_y = _mm_mul_ps( planes0_y, aabb_max_y );
	__m128 const aabb_max_p0_z = _mm_mul_ps( planes0_z, aabb_max_z );

	__m128 const aabb_min_p1_x = _mm_mul_ps( planes1_x, aabb_min_x );
	__m128 const aabb_min_p1_y = _mm_mul_ps( planes1_y, aabb_min_y );
	__m128 const aabb_min_p1_z = _mm_mul_ps( planes1_z, aabb_min_z );

	__m128 const aabb_max_p1_x = _mm_mul_ps( planes1_x, aabb_max_x );
	__m128 const aabb_max_p1_y = _mm_mul_ps( planes1_y, aabb_max_y );
	__m128 const aabb_max_p1_z = _mm_mul_ps( planes1_z, aabb_max_z );

	__m128i const dot_compare_to_zero_mask = _mm_setr_epi32( 0x80000000, 0x80000000, 0x80000000, 0 );

	{
		__m128 const aabb_p0_x = _mm_min_ps( aabb_min_p0_x, aabb_max_p0_x );
		__m128 const aabb_p0_y = _mm_min_ps( aabb_min_p0_y, aabb_max_p0_y );
		__m128 const aabb_p0_z = _mm_min_ps( aabb_min_p0_z, aabb_max_p0_z );

		__m128 const aabb_p1_x = _mm_min_ps( aabb_min_p1_x, aabb_max_p1_x );
		__m128 const aabb_p1_y = _mm_min_ps( aabb_min_p1_y, aabb_max_p1_y );
		__m128 const aabb_p1_z = _mm_min_ps( aabb_min_p1_z, aabb_max_p1_z );

		__m128 const dots_0 = _mm_add_ps( _mm_add_ps( aabb_p0_x, aabb_p0_y ), _mm_add_ps( aabb_p0_z, planes0_w ) );
		__m128 const dots_1 = _mm_add_ps( _mm_add_ps( aabb_p1_x, aabb_p1_y ), _mm_add_ps( aabb_p1_z, planes1_w ) );

		__m128 const dots = _mm_or_ps( dots_0, dots_1 );

		if ( _mm_testz_si128( _mm_castps_si128( dots ), dot_compare_to_zero_mask ) )
			return true;

		return false;
	}
}

// true, if AABB is fully outside the frustum
bool frustum_aligned::test_aabb_outside( aabb_aligned const& box ) const
{
	__m128 const aabb_min = box.m_min;
	__m128 const aabb_max = box.m_max;

	__m128 const aabb_min_x = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_min_y = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_min_z = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	
	__m128 const aabb_max_x = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_max_y = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_max_z = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) );

	__m128 const planes0_x = planes[0];
	__m128 const planes0_y = planes[1];
	__m128 const planes0_z = planes[2];
	__m128 const planes0_w = planes[3];
	__m128 const planes1_x = planes[4];
	__m128 const planes1_y = planes[5];
	__m128 const planes1_z = planes[6];
	__m128 const planes1_w = planes[7];
	
	__m128 const aabb_min_p0_x = _mm_mul_ps( planes0_x, aabb_min_x );
	__m128 const aabb_min_p0_y = _mm_mul_ps( planes0_y, aabb_min_y );
	__m128 const aabb_min_p0_z = _mm_mul_ps( planes0_z, aabb_min_z );

	__m128 const aabb_max_p0_x = _mm_mul_ps( planes0_x, aabb_max_x );
	__m128 const aabb_max_p0_y = _mm_mul_ps( planes0_y, aabb_max_y );
	__m128 const aabb_max_p0_z = _mm_mul_ps( planes0_z, aabb_max_z );

	__m128 const aabb_min_p1_x = _mm_mul_ps( planes1_x, aabb_min_x );
	__m128 const aabb_min_p1_y = _mm_mul_ps( planes1_y, aabb_min_y );
	__m128 const aabb_min_p1_z = _mm_mul_ps( planes1_z, aabb_min_z );

	__m128 const aabb_max_p1_x = _mm_mul_ps( planes1_x, aabb_max_x );
	__m128 const aabb_max_p1_y = _mm_mul_ps( planes1_y, aabb_max_y );
	__m128 const aabb_max_p1_z = _mm_mul_ps( planes1_z, aabb_max_z );

	__m128i const dot_compare_to_zero_mask = _mm_setr_epi32( 0x80000000, 0x80000000, 0x80000000, 0 );

	{
		__m128 const aabb_p0_x = _mm_max_ps( aabb_min_p0_x, aabb_max_p0_x );
		__m128 const aabb_p0_y = _mm_max_ps( aabb_min_p0_y, aabb_max_p0_y );
		__m128 const aabb_p0_z = _mm_max_ps( aabb_min_p0_z, aabb_max_p0_z );

		__m128 const aabb_p1_x = _mm_max_ps( aabb_min_p1_x, aabb_max_p1_x );
		__m128 const aabb_p1_y = _mm_max_ps( aabb_min_p1_y, aabb_max_p1_y );
		__m128 const aabb_p1_z = _mm_max_ps( aabb_min_p1_z, aabb_max_p1_z );

		__m128 const dots_0 = _mm_add_ps( _mm_add_ps( aabb_p0_x, aabb_p0_y ), _mm_add_ps( aabb_p0_z, planes0_w ) );
		__m128 const dots_1 = _mm_add_ps( _mm_add_ps( aabb_p1_x, aabb_p1_y ), _mm_add_ps( aabb_p1_z, planes1_w ) );

		__m128 const dots = _mm_or_ps( dots_0, dots_1 );

		if ( !_mm_testz_si128( _mm_castps_si128( dots ), dot_compare_to_zero_mask ) )
				return true;

		return false;
	}
}

intersection frustum_aligned::test_aabb( aabb_aligned const& box ) const
{
	__m128 const aabb_min = box.m_min;
	__m128 const aabb_max = box.m_max;

	__m128 const aabb_min_x = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_min_y = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_min_z = _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	
	__m128 const aabb_max_x = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	__m128 const aabb_max_y = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 const aabb_max_z = _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) );

	__m128 const planes0_x = planes[0];
	__m128 const planes0_y = planes[1];
	__m128 const planes0_z = planes[2];
	__m128 const planes0_w = planes[3];
	__m128 const planes1_x = planes[4];
	__m128 const planes1_y = planes[5];
	__m128 const planes1_z = planes[6];
	__m128 const planes1_w = planes[7];
	
	__m128 const aabb_min_p0_x = _mm_mul_ps( planes0_x, aabb_min_x );
	__m128 const aabb_min_p0_y = _mm_mul_ps( planes0_y, aabb_min_y );
	__m128 const aabb_min_p0_z = _mm_mul_ps( planes0_z, aabb_min_z );

	__m128 const aabb_max_p0_x = _mm_mul_ps( planes0_x, aabb_max_x );
	__m128 const aabb_max_p0_y = _mm_mul_ps( planes0_y, aabb_max_y );
	__m128 const aabb_max_p0_z = _mm_mul_ps( planes0_z, aabb_max_z );

	__m128 const aabb_min_p1_x = _mm_mul_ps( planes1_x, aabb_min_x );
	__m128 const aabb_min_p1_y = _mm_mul_ps( planes1_y, aabb_min_y );
	__m128 const aabb_min_p1_z = _mm_mul_ps( planes1_z, aabb_min_z );

	__m128 const aabb_max_p1_x = _mm_mul_ps( planes1_x, aabb_max_x );
	__m128 const aabb_max_p1_y = _mm_mul_ps( planes1_y, aabb_max_y );
	__m128 const aabb_max_p1_z = _mm_mul_ps( planes1_z, aabb_max_z );

	__m128i const dot_compare_to_zero_mask = _mm_setr_epi32( 0x80000000, 0x80000000, 0x80000000, 0 );

	{
		__m128 const aabb_p0_x = _mm_max_ps( aabb_min_p0_x, aabb_max_p0_x );
		__m128 const aabb_p0_y = _mm_max_ps( aabb_min_p0_y, aabb_max_p0_y );
		__m128 const aabb_p0_z = _mm_max_ps( aabb_min_p0_z, aabb_max_p0_z );

		__m128 const aabb_p1_x = _mm_max_ps( aabb_min_p1_x, aabb_max_p1_x );
		__m128 const aabb_p1_y = _mm_max_ps( aabb_min_p1_y, aabb_max_p1_y );
		__m128 const aabb_p1_z = _mm_max_ps( aabb_min_p1_z, aabb_max_p1_z );

		__m128 const dots_0 = _mm_add_ps( _mm_add_ps( aabb_p0_x, aabb_p0_y ), _mm_add_ps( aabb_p0_z, planes0_w ) );
		__m128 const dots_1 = _mm_add_ps( _mm_add_ps( aabb_p1_x, aabb_p1_y ), _mm_add_ps( aabb_p1_z, planes1_w ) );

		__m128 const dots = _mm_or_ps( dots_0, dots_1 );

		if ( !_mm_testz_si128( _mm_castps_si128( dots ), dot_compare_to_zero_mask ) )
				return outside;
	}
	
	{
		__m128 const aabb_p0_x = _mm_min_ps( aabb_min_p0_x, aabb_max_p0_x );
		__m128 const aabb_p0_y = _mm_min_ps( aabb_min_p0_y, aabb_max_p0_y );
		__m128 const aabb_p0_z = _mm_min_ps( aabb_min_p0_z, aabb_max_p0_z );

		__m128 const aabb_p1_x = _mm_min_ps( aabb_min_p1_x, aabb_max_p1_x );
		__m128 const aabb_p1_y = _mm_min_ps( aabb_min_p1_y, aabb_max_p1_y );
		__m128 const aabb_p1_z = _mm_min_ps( aabb_min_p1_z, aabb_max_p1_z );

		__m128 const dots_0 = _mm_add_ps( _mm_add_ps( aabb_p0_x, aabb_p0_y ), _mm_add_ps( aabb_p0_z, planes0_w ) );
		__m128 const dots_1 = _mm_add_ps( _mm_add_ps( aabb_p1_x, aabb_p1_y ), _mm_add_ps( aabb_p1_z, planes1_w ) );

		__m128 const dots = _mm_or_ps( dots_0, dots_1 );

		if ( _mm_testz_si128( _mm_castps_si128( dots ), dot_compare_to_zero_mask ) )
			return inside;

		return intersect;
	}
}

} // namespace math