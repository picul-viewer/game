#include "aabb.h"

#include <macros.h>
#include <math/math_common.h>

aabb& aabb::get_box( )
{
	return *this;
}

aabb const& aabb::get_box( ) const
{
	return *this;
}

void aabb::set_min_max( math::float3 const& min, math::float3 const& max )
{
	this->min = min;
	this->max = max;
}

void aabb::set_center_radius( math::float3 const& center, math::float3 const& radius )
{
	ASSERT( radius > 0 );
	this->min = center - radius;
	this->max = center + radius;
}

math::float3 aabb::center( ) const
{
	return ( min + max ) * 0.5f;
}

math::float3 aabb::radius( ) const
{
	return ( max - min ) * 0.5f;
}

void aabb::extrude( math::float3 const& point )
{
	min = math::min( min, point );
	max = math::max( max, point );
}

void aabb::extrude( aabb const& box )
{
	min = math::min( min, box.min );
	max = math::max( max, box.max );
}

void aabb::modify( math::float3 const& transform )
{
	min += transform;
	max += transform;
}

float aabb::surface_area( ) const
{
	math::float3 v = max - min;

	return 2 * ( v.x * v.y + v.y * v.z + v.z * v.x );
}

void aabb::modify( math::float3x3 const& transform )
{
	math::float3 const& min_l0 = min * transform.l0;
	math::float3 const& max_l0 = max * transform.l0;

	math::float3 new_min;
	math::float3 new_max;

	new_min.x = math::min( min_l0.x, max_l0.x ) +
				math::min( min_l0.y, max_l0.y ) +
				math::min( min_l0.z, max_l0.z );
	new_max.x = math::max( min_l0.x, max_l0.x ) +
				math::max( min_l0.y, max_l0.y ) +
				math::max( min_l0.z, max_l0.z );

	math::float3 const& min_l1 = min * transform.l1;
	math::float3 const& max_l1 = max * transform.l1;

	new_min.y = math::min( min_l1.x, max_l1.x ) +
				math::min( min_l1.y, max_l1.y ) +
				math::min( min_l1.z, max_l1.z );
	new_max.y = math::max( min_l1.x, max_l1.x ) +
				math::max( min_l1.y, max_l1.y ) +
				math::max( min_l1.z, max_l1.z );

	math::float3 const& min_l2 = min * transform.l2;
	math::float3 const& max_l2 = max * transform.l2;

	new_min.z = math::min( min_l2.x, max_l2.x ) +
				math::min( min_l2.y, max_l2.y ) +
				math::min( min_l2.z, max_l2.z );
	new_max.z = math::max( min_l2.x, max_l2.x ) +
				math::max( min_l2.y, max_l2.y ) +
				math::max( min_l2.z, max_l2.z );
	
	min = new_min;
	max = new_max;
}

void aabb::modify( math::float4x3 const& transform )
{
	math::float3 const& min_l0 = min * transform.l0.vx;
	math::float3 const& max_l0 = max * transform.l0.vx;

	math::float3 new_min;
	math::float3 new_max;

	new_min.x = math::min( min_l0.x, max_l0.x ) +
				math::min( min_l0.y, max_l0.y ) +
				math::min( min_l0.z, max_l0.z ) + transform.l0.w;
	new_max.x = math::max( min_l0.x, max_l0.x ) +
				math::max( min_l0.y, max_l0.y ) +
				math::max( min_l0.z, max_l0.z ) + transform.l0.w;

	math::float3 const& min_l1 = min * transform.l1.vx;
	math::float3 const& max_l1 = max * transform.l1.vx;
	
	new_min.y = math::min( min_l1.x, max_l1.x ) +
				math::min( min_l1.y, max_l1.y ) +
				math::min( min_l1.z, max_l1.z ) + transform.l1.w;
	new_max.y = math::max( min_l1.x, max_l1.x ) +
				math::max( min_l1.y, max_l1.y ) +
				math::max( min_l1.z, max_l1.z ) + transform.l1.w;

	math::float3 const& min_l2 = min * transform.l2.vx;
	math::float3 const& max_l2 = max * transform.l2.vx;
	
	new_min.z = math::min( min_l2.x, max_l2.x ) +
				math::min( min_l2.y, max_l2.y ) +
				math::min( min_l2.z, max_l2.z ) + transform.l2.w;
	new_max.z = math::max( min_l2.x, max_l2.x ) +
				math::max( min_l2.y, max_l2.y ) +
				math::max( min_l2.z, max_l2.z ) + transform.l2.w;

	min = new_min;
	max = new_max;
}


aabb_aligned::aabb_aligned( )
{
	ASSERT( aligned( this, 16 ) );
}

aabb& aabb_aligned::get_box( )
{
	return *(aabb*)this;
}

aabb const& aabb_aligned::get_box( ) const
{
	return *(aabb const*)this;
}

void aabb_aligned::set_min_max( math::sse::vector const& min, math::sse::vector const& max )
{
	this->min = min;
	this->max = max;
}

void aabb_aligned::set_center_radius( math::sse::vector const& center, math::sse::vector const& radius )
{
	min = _mm_sub_ps( center, radius );
	max = _mm_add_ps( center, radius );
}

math::sse::vector aabb_aligned::center( ) const
{
	__m128 sum = _mm_add_ps( min, max );
	__m128 res = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );

	return res;
}

math::sse::vector aabb_aligned::radius( ) const
{
	__m128 dif = _mm_sub_ps( max, min );
	__m128 res = _mm_mul_ps( dif, _mm_set1_ps( 0.5f ) );

	return res;
}

void aabb_aligned::get_center_radius( math::sse::vector& center, math::sse::vector& radius )
{
	__m128 sum = _mm_add_ps( min, max );

	center = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );
	radius = _mm_sub_ps( max, center );
}

void aabb_aligned::extrude( math::sse::vector const& point )
{
	min = _mm_min_ps( min, point );
	max = _mm_max_ps( max, point );
}

void aabb_aligned::extrude( aabb_aligned const& box )
{
	min = _mm_min_ps( min, box.min );
	max = _mm_max_ps( max, box.max );
}

void aabb_aligned::modify( math::sse::vector const& transform )
{
	min = _mm_add_ps( min, transform );
	max = _mm_add_ps( max, transform );
}

float aabb_aligned::surface_area( ) const
{
	__m128 bones = _mm_sub_ps( max, min );
	__m128 mbones = _mm_shuffle_ps( bones, bones, _MM_SHUFFLE( 3, 0, 2, 1 ) );

	__m128 squares = _mm_mul_ps( bones, mbones );
	__m128 sq1 = _mm_shuffle_ps( squares, squares, _MM_SHUFFLE( 1, 1, 1, 1 ) );
	__m128 sq2 = _mm_shuffle_ps( squares, squares, _MM_SHUFFLE( 2, 2, 2, 2 ) );
	return _mm_mul_ss( _mm_add_ss( _mm_add_ss( squares, sq1 ), sq2 ), _mm_set_ss( 2.0f ) ).m128_f32[0];
}

void aabb_aligned::modify( math::sse::matrix3 const& transform )
{
	__m128 l0_l = _mm_mul_ps( transform[0], min );
	__m128 l0_h = _mm_mul_ps( transform[0], max );

	__m128 l0_min = _mm_min_ps( l0_l, l0_h );
	__m128 l0_max = _mm_max_ps( l0_l, l0_h );

	__m128 l1_l = _mm_mul_ps( transform[1], min );
	__m128 l1_h = _mm_mul_ps( transform[1], max );

	__m128 l1_min = _mm_min_ps( l1_l, l1_h );
	__m128 l1_max = _mm_max_ps( l1_l, l1_h );
	
	__m128 l2_l = _mm_mul_ps( transform[2], min );
	__m128 l2_h = _mm_mul_ps( transform[2], max );

	__m128 l2_min = _mm_min_ps( l2_l, l2_h );
	__m128 l2_max = _mm_max_ps( l2_l, l2_h );

	__m128 min_x = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 0, 0, 0, 0 ) ), l2_min, _MM_SHUFFLE( 0, 0, 2, 0 ) );
	__m128 min_y = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 1, 1, 1, 1 ) ), l2_min, _MM_SHUFFLE( 1, 1, 2, 0 ) );
	__m128 min_z = _mm_shuffle_ps( _mm_shuffle_ps( l0_min, l1_min, _MM_SHUFFLE( 2, 2, 2, 2 ) ), l2_min, _MM_SHUFFLE( 2, 2, 2, 0 ) );
	
	__m128 max_x = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 0, 0, 0, 0 ) ), l2_max, _MM_SHUFFLE( 0, 0, 2, 0 ) );
	__m128 max_y = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 1, 1, 1, 1 ) ), l2_max, _MM_SHUFFLE( 1, 1, 2, 0 ) );
	__m128 max_z = _mm_shuffle_ps( _mm_shuffle_ps( l0_max, l1_max, _MM_SHUFFLE( 2, 2, 2, 2 ) ), l2_max, _MM_SHUFFLE( 2, 2, 2, 0 ) );

	__m128 trans = _mm_shuffle_ps( _mm_shuffle_ps( transform[0], transform[1], _MM_SHUFFLE( 3, 3, 3, 3 ) ), transform[2], _MM_SHUFFLE( 3, 3, 2, 0 ) );

	min = _mm_add_ps( _mm_add_ps( min_x, min_y ), _mm_add_ps( min_z, trans ) );
	max = _mm_add_ps( _mm_add_ps( max_x, max_y ), _mm_add_ps( max_z, trans ) );
}