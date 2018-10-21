#include "aabb.h"

#include <macros.h>
#include "math_common.h"

namespace math {

aabb aabb::get_box( ) const
{
	return *this;
}

void aabb::set_min_max( float3 const& min, float3 const& max )
{
	m_min = min;
	m_max = max;
}

void aabb::set_center_radius( float3 const& center, float3 const& radius )
{
	ASSERT( radius > 0.0f );
	m_min = center - radius;
	m_max = center + radius;
}

float3 aabb::center( ) const
{
	return ( m_min + m_max ) * 0.5f;
}

float3 aabb::radius( ) const
{
	return ( m_max - m_min ) * 0.5f;
}

float3 aabb::get_min( ) const
{
	return m_min;
}

float3 aabb::get_max( ) const
{
	return m_max;
}

void aabb::extrude( float3 const& point )
{
	m_min = min( m_min, point );
	m_max = max( m_max, point );
}

void aabb::extrude( aabb const& box )
{
	m_min = min( m_min, box.m_min );
	m_max = max( m_max, box.m_max );
}

void aabb::modify( float3 const& transform )
{
	m_min += transform;
	m_max += transform;
}

float aabb::surface_area( ) const
{
	float3 v = m_max - m_min;

	return 2 * ( v.x * v.y + v.y * v.z + v.z * v.x );
}

void aabb::modify( float3x3 const& transform )
{
	float3 const& min_l0 = m_min * transform.l0;
	float3 const& max_l0 = m_max * transform.l0;

	float3 new_min;
	float3 new_max;

	new_min.x = min( min_l0.x, max_l0.x ) +
				min( min_l0.y, max_l0.y ) +
				min( min_l0.z, max_l0.z );
	new_max.x = max( min_l0.x, max_l0.x ) +
				max( min_l0.y, max_l0.y ) +
				max( min_l0.z, max_l0.z );

	float3 const& min_l1 = m_min * transform.l1;
	float3 const& max_l1 = m_max * transform.l1;

	new_min.y = min( min_l1.x, max_l1.x ) +
				min( min_l1.y, max_l1.y ) +
				min( min_l1.z, max_l1.z );
	new_max.y = max( min_l1.x, max_l1.x ) +
				max( min_l1.y, max_l1.y ) +
				max( min_l1.z, max_l1.z );

	float3 const& min_l2 = m_min * transform.l2;
	float3 const& max_l2 = m_max * transform.l2;

	new_min.z = min( min_l2.x, max_l2.x ) +
				min( min_l2.y, max_l2.y ) +
				min( min_l2.z, max_l2.z );
	new_max.z = max( min_l2.x, max_l2.x ) +
				max( min_l2.y, max_l2.y ) +
				max( min_l2.z, max_l2.z );
	
	m_min = new_min;
	m_max = new_max;
}

void aabb::modify( float4x3 const& transform )
{
	float3 const& min_l0 = m_min * transform.l0.vx;
	float3 const& max_l0 = m_max * transform.l0.vx;

	float3 new_min;
	float3 new_max;

	new_min.x = min( min_l0.x, max_l0.x ) +
				min( min_l0.y, max_l0.y ) +
				min( min_l0.z, max_l0.z ) + transform.l0.w;
	new_max.x = max( min_l0.x, max_l0.x ) +
				max( min_l0.y, max_l0.y ) +
				max( min_l0.z, max_l0.z ) + transform.l0.w;

	float3 const& min_l1 = m_min * transform.l1.vx;
	float3 const& max_l1 = m_max * transform.l1.vx;
	
	new_min.y = min( min_l1.x, max_l1.x ) +
				min( min_l1.y, max_l1.y ) +
				min( min_l1.z, max_l1.z ) + transform.l1.w;
	new_max.y = max( min_l1.x, max_l1.x ) +
				max( min_l1.y, max_l1.y ) +
				max( min_l1.z, max_l1.z ) + transform.l1.w;

	float3 const& min_l2 = m_min * transform.l2.vx;
	float3 const& max_l2 = m_max * transform.l2.vx;
	
	new_min.z = min( min_l2.x, max_l2.x ) +
				min( min_l2.y, max_l2.y ) +
				min( min_l2.z, max_l2.z ) + transform.l2.w;
	new_max.z = max( min_l2.x, max_l2.x ) +
				max( min_l2.y, max_l2.y ) +
				max( min_l2.z, max_l2.z ) + transform.l2.w;

	m_min = new_min;
	m_max = new_max;
}


aabb_aligned::aabb_aligned( )
{
	ASSERT( aligned( this, 16 ) );
}

aabb aabb_aligned::get_box( ) const
{
	aabb box;
	box.set_min_max( (math::float4)m_min, (math::float4)m_max );
	return box;
}

void aabb_aligned::set_min_max( sse::vector const& min, sse::vector const& max )
{
	m_min = min;
	m_max = max;
}

void aabb_aligned::set_center_radius( sse::vector const& center, sse::vector const& radius )
{
	m_min = _mm_sub_ps( center, radius );
	m_max = _mm_add_ps( center, radius );
}

sse::vector aabb_aligned::center( ) const
{
	__m128 sum = _mm_add_ps( m_min, m_max );
	__m128 res = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );

	return res;
}

sse::vector aabb_aligned::radius( ) const
{
	__m128 dif = _mm_sub_ps( m_max, m_min );
	__m128 res = _mm_mul_ps( dif, _mm_set1_ps( 0.5f ) );

	return res;
}

void aabb_aligned::get_center_radius( sse::vector& center, sse::vector& radius )
{
	__m128 sum = _mm_add_ps( m_min, m_max );

	center = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );
	radius = _mm_sub_ps( m_max, center );
}

float3 aabb_aligned::get_min( ) const
{
	return float3( m_min.data.m128_f32[0], m_min.data.m128_f32[1], m_min.data.m128_f32[2] );
}

float3 aabb_aligned::get_max( ) const
{
	return float3( m_max.data.m128_f32[0], m_max.data.m128_f32[1], m_max.data.m128_f32[2] );
}

void aabb_aligned::extrude( sse::vector const& point )
{
	m_min = _mm_min_ps( m_min, point );
	m_max = _mm_max_ps( m_max, point );
}

void aabb_aligned::extrude( aabb_aligned const& box )
{
	m_min = _mm_min_ps( m_min, box.m_min );
	m_max = _mm_max_ps( m_max, box.m_max );
}

void aabb_aligned::modify( sse::vector const& transform )
{
	m_min = _mm_add_ps( m_min, transform );
	m_max = _mm_add_ps( m_max, transform );
}

float aabb_aligned::surface_area( ) const
{
	__m128 bones = _mm_sub_ps( m_max, m_min );
	__m128 mbones = _mm_shuffle_ps( bones, bones, _MM_SHUFFLE( 3, 0, 2, 1 ) );

	__m128 squares = _mm_mul_ps( bones, mbones );
	__m128 sq1 = _mm_shuffle_ps( squares, squares, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 sq2 = _mm_shuffle_ps( squares, squares, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	return _mm_mul_ss( _mm_add_ss( _mm_add_ss( squares, sq1 ), sq2 ), _mm_set_ss( 2.0f ) ).m128_f32[0];
}

void aabb_aligned::modify( sse::matrix3 const& transform )
{
	__m128 l0_l = _mm_mul_ps( transform[0], m_min );
	__m128 l0_h = _mm_mul_ps( transform[0], m_max );

	__m128 l0_min = _mm_min_ps( l0_l, l0_h );
	__m128 l0_max = _mm_max_ps( l0_l, l0_h );

	__m128 l1_l = _mm_mul_ps( transform[1], m_min );
	__m128 l1_h = _mm_mul_ps( transform[1], m_max );

	__m128 l1_min = _mm_min_ps( l1_l, l1_h );
	__m128 l1_max = _mm_max_ps( l1_l, l1_h );
	
	__m128 l2_l = _mm_mul_ps( transform[2], m_min );
	__m128 l2_h = _mm_mul_ps( transform[2], m_max );

	__m128 l2_min = _mm_min_ps( l2_l, l2_h );
	__m128 l2_max = _mm_max_ps( l2_l, l2_h );

	__m128 min_x = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 0, 0, 0, 0 ) ), l2_min, _MM_SHUFFLE( 0, 0, 2, 0 ) );
	__m128 min_y = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 1, 1, 1, 1 ) ), l2_min, _MM_SHUFFLE( 1, 1, 2, 0 ) );
	__m128 min_z = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 2, 2, 2, 2 ) ), l2_min, _MM_SHUFFLE( 2, 2, 2, 0 ) );
	
	__m128 max_x = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 0, 0, 0, 0 ) ), l2_max, _MM_SHUFFLE( 0, 0, 2, 0 ) );
	__m128 max_y = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 1, 1, 1, 1 ) ), l2_max, _MM_SHUFFLE( 1, 1, 2, 0 ) );
	__m128 max_z = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 2, 2, 2, 2 ) ), l2_max, _MM_SHUFFLE( 2, 2, 2, 0 ) );

	__m128 trans = _mm_shuffle_ps( _mm_shuffle_ps( transform[0], transform[1], _MM_SHUFFLE( 3, 3, 3, 3 ) ), transform[2], _MM_SHUFFLE( 3, 3, 2, 0 ) );

	m_min = _mm_add_ps( _mm_add_ps( min_x, min_y ), _mm_add_ps( min_z, trans ) );
	m_max = _mm_add_ps( _mm_add_ps( max_x, max_y ), _mm_add_ps( max_z, trans ) );
}

} // namespace math