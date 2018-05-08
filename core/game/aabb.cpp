#include "aabb.h"

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