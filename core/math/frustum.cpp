#include "frustum.h"

frustum::plane::plane( )
{ }

void frustum::set_from_matrix( math::float4x4 const& m )
{
	planes[left_plane].vector	= math::float4(  m.a00 + m.a03,  m.a10 + m.a13,  m.a20 + m.a23,  m.a30 + m.a33 );
	planes[right_plane].vector	= math::float4( -m.a00 + m.a03, -m.a10 + m.a13, -m.a20 + m.a23, -m.a30 + m.a33 );

	planes[bottom_plane].vector	= math::float4(  m.a01 + m.a03,  m.a11 + m.a13,  m.a21 + m.a23,  m.a31 + m.a33 );
	planes[top_plane].vector	= math::float4( -m.a01 + m.a03, -m.a11 + m.a13, -m.a21 + m.a23, -m.a31 + m.a33 );

	planes[near_plane].vector	= math::float4(			 m.a03,			 m.a13,			 m.a23,			 m.a33 );
	planes[far_plane].vector	= math::float4( -m.a02 + m.a03, -m.a12 + m.a13, -m.a22 + m.a23, -m.a32 + m.a33 );
}

// true, if AABB is fully inside the frustum
bool frustum::plane::test_aabb_inside( aabb const& box ) const
{
	math::float3 const max_v( ( n.x < 0 ) ? box.min.x : box.max.x,
							  ( n.y < 0 ) ? box.min.y : box.max.y, 
							  ( n.z < 0 ) ? box.min.z : box.max.z );

	float const dot2 = max_v.dot( n );

	return dot2 >= -d;
}

// true, if AABB is fully outside the frustum
bool frustum::plane::test_aabb_outside( aabb const& box ) const
{
	math::float3 const min_v( ( n.x > 0 ) ? box.min.x : box.max.x,
							  ( n.y > 0 ) ? box.min.y : box.max.y, 
							  ( n.z > 0 ) ? box.min.z : box.max.z );

	float const dot = min_v.dot( n );

	return dot > -d;
}

intersection frustum::plane::test_aabb( aabb const& box ) const
{
	math::float3 const min_v( ( n.x > 0 ) ? box.min.x : box.max.x,
							  ( n.y > 0 ) ? box.min.y : box.max.y, 
							  ( n.z > 0 ) ? box.min.z : box.max.z );

	float const dot = min_v.dot( n );

	if ( dot > -d )
		return outside;

	math::float3 const max_v( ( n.x < 0 ) ? box.min.x : box.max.x,
							  ( n.y < 0 ) ? box.min.y : box.max.y, 
							  ( n.z < 0 ) ? box.min.z : box.max.z );

	float const dot2 = max_v.dot( n );

	if ( dot2 >= -d )
		return intersect;

	return inside;
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
	u32 intersected = 0;

	for ( u32 i = 0; i < plane_count; ++i )
	{
		switch ( planes[i].test_aabb( box ) )
		{
		case inside: continue;
		case intersect: intersected = 1; continue;
		case outside: return outside;
		default:
			UNREACHABLE_CODE;
		}
	}

	return (intersection)( inside + intersected ); // ( intersected ) ? intersect : inside
}


frustum_aligned::frustum_aligned( )
{
	ASSERT( aligned( this, 16 ) );
}

void frustum_aligned::set_from_matrix( math::sse::matrix const& m )
{
	ASSERT( aligned( m.data, 16 ) );

	this->m = m;
}

// true, if AABB is fully inside the frustum
bool frustum_aligned::test_aabb_inside( aabb_aligned const& box ) const
{
	ASSERT( aligned( &box, 16 ) );

	__m128 aabb_min = box.min;
	__m128 aabb_max = box.max;

	__m128 aabb_min_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_min_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_min_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );
	
	__m128 aabb_max_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_max_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_max_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );

	__m128 min_l0 = _mm_min_ps( aabb_min_l0, aabb_max_l0 );
	__m128 min_l1 = _mm_min_ps( aabb_min_l1, aabb_max_l1 );
	__m128 min_l2 = _mm_min_ps( aabb_min_l2, aabb_max_l2 );
	
	__m128 max_l0 = _mm_max_ps( aabb_min_l0, aabb_max_l0 );
	__m128 max_l1 = _mm_max_ps( aabb_min_l1, aabb_max_l1 );
	__m128 max_l2 = _mm_max_ps( aabb_min_l2, aabb_max_l2 );
	
	__m128 min_dots = _mm_add_ps( _mm_add_ps( min_l0, min_l1 ), _mm_add_ps( min_l2, m[3] ) );
	__m128 max_dots = _mm_add_ps( _mm_add_ps( max_l0, max_l1 ), _mm_add_ps( max_l2, m[3] ) );

	__m128 max_dot_c3 = _mm_shuffle_ps( max_dots, max_dots, _MM_SHUFFLE( 3, 3, 3, 3 ) );

	__m128i dot_compare_to_zero_mask = _mm_setr_epi32( INT_MIN, INT_MIN, INT_MIN, 0 );
	__m128 dot_c3_addition_mask = _mm_castsi128_ps( _mm_setr_epi32( -1, -1, 0, 0 ) );

	{
		// maximum dot products, comparing to zero
		// can compare to zero by manipulating MSB, see unaligned frustum implementation for exact meaning
		// don't care about 'more-equal than' here, just replace it with 'more than'
		// simultaneously we can discard result of comparing 4th and avoid correcting integer mask
		__m128 cmp0 = _mm_add_ps( max_dot_c3, _mm_and_ps( max_dots, dot_c3_addition_mask ) );
		__m128 cmp1 = _mm_sub_ps( max_dot_c3, min_dots );
		
		__m128 comb = _mm_and_ps( cmp0, cmp1 );

		return !!_mm_testc_si128( _mm_castps_si128( comb ), dot_compare_to_zero_mask );
	}
}

// true, if AABB is fully outside the frustum
bool frustum_aligned::test_aabb_outside( aabb_aligned const& box ) const
{
	ASSERT( aligned( &box, 16 ) );

	__m128 aabb_min = box.min;
	__m128 aabb_max = box.max;
	
	__m128 aabb_min_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_min_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_min_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );
	
	__m128 aabb_max_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_max_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_max_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );

	__m128 min_l0 = _mm_min_ps( aabb_min_l0, aabb_max_l0 );
	__m128 min_l1 = _mm_min_ps( aabb_min_l1, aabb_max_l1 );
	__m128 min_l2 = _mm_min_ps( aabb_min_l2, aabb_max_l2 );
	
	__m128 max_l0 = _mm_max_ps( aabb_min_l0, aabb_max_l0 );
	__m128 max_l1 = _mm_max_ps( aabb_min_l1, aabb_max_l1 );
	__m128 max_l2 = _mm_max_ps( aabb_min_l2, aabb_max_l2 );

	__m128 min_dots = _mm_add_ps( _mm_add_ps( min_l0, min_l1 ), _mm_add_ps( min_l2, m[3] ) );
	__m128 max_dots = _mm_add_ps( _mm_add_ps( max_l0, max_l1 ), _mm_add_ps( max_l2, m[3] ) );

	__m128 min_dot_c3 = _mm_shuffle_ps( min_dots, min_dots, _MM_SHUFFLE( 3, 3, 3, 3 ) );

	__m128i dot_compare_to_zero_mask = _mm_setr_epi32( INT_MIN, INT_MIN, INT_MIN, 0 );
	__m128 dot_c3_addition_mask = _mm_castsi128_ps( _mm_setr_epi32( -1, -1, 0, 0 ) );

	{
		// minimum dot products, comparing to zero
		// can compare to zero by manipulating MSB, see unaligned frustum implementation for exact meaning
		// simultaneously we can discard result of comparing 4th element and avoid correcting integer mask
		__m128 cmp0 = _mm_add_ps( min_dot_c3, _mm_and_ps( min_dots, dot_c3_addition_mask ) );
		__m128 cmp1 = _mm_sub_ps( min_dot_c3, max_dots );
		
		__m128 comb = _mm_and_ps( cmp0, cmp1 );

		return !_mm_testc_si128( _mm_castps_si128( comb ), dot_compare_to_zero_mask );
	}
}

intersection frustum_aligned::test_aabb( aabb_aligned const& box ) const
{
	ASSERT( aligned( &box, 16 ) );

	__m128 aabb_min = box.min;
	__m128 aabb_max = box.max;

	__m128 aabb_min_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_min_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_min_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_min, aabb_min, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );
	
	__m128 aabb_max_l0 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 0, 0, 0, 0 ) ), m[0] );
	__m128 aabb_max_l1 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 1, 1, 1, 1 ) ), m[1] );
	__m128 aabb_max_l2 = _mm_mul_ps( _mm_shuffle_ps( aabb_max, aabb_max, _MM_SHUFFLE( 2, 2, 2, 2 ) ), m[2] );

	__m128 min_l0 = _mm_min_ps( aabb_min_l0, aabb_max_l0 );
	__m128 min_l1 = _mm_min_ps( aabb_min_l1, aabb_max_l1 );
	__m128 min_l2 = _mm_min_ps( aabb_min_l2, aabb_max_l2 );
	
	__m128 max_l0 = _mm_max_ps( aabb_min_l0, aabb_max_l0 );
	__m128 max_l1 = _mm_max_ps( aabb_min_l1, aabb_max_l1 );
	__m128 max_l2 = _mm_max_ps( aabb_min_l2, aabb_max_l2 );
	
	__m128 min_dots = _mm_add_ps( _mm_add_ps( min_l0, min_l1 ), _mm_add_ps( min_l2, m[3] ) );
	__m128 max_dots = _mm_add_ps( _mm_add_ps( max_l0, max_l1 ), _mm_add_ps( max_l2, m[3] ) );

	__m128 min_dot_c3 = _mm_shuffle_ps( min_dots, min_dots, _MM_SHUFFLE( 3, 3, 3, 3 ) );
	__m128 max_dot_c3 = _mm_shuffle_ps( max_dots, max_dots, _MM_SHUFFLE( 3, 3, 3, 3 ) );
	
	__m128i dot_compare_to_zero_mask = _mm_setr_epi32( INT_MIN, INT_MIN, INT_MIN, 0 );
	__m128 dot_addition_mask = _mm_castsi128_ps( _mm_setr_epi32( -1, -1, 0, 0 ) );

	{
		// minimum dot products, comparing to zero
		// can compare to zero by manipulating MSB, see unaligned frustum implementation for exact meaning
		// simultaneously we can discard result of comparing 4th element and avoid correcting integer mask
		__m128 cmp0 = _mm_add_ps( min_dot_c3, _mm_and_ps( min_dots, dot_addition_mask ) );
		__m128 cmp1 = _mm_sub_ps( min_dot_c3, max_dots );

		__m128 comb = _mm_and_ps( cmp0, cmp1 );

		if ( !_mm_testc_si128( _mm_castps_si128( comb ), dot_compare_to_zero_mask ) )
			return outside;
	}
	
	{
		// maximum dot products, comparing to zero
		// can compare to zero by manipulating MSB, see unaligned frustum implementation for exact meaning
		// don't care about 'more-equal than' here, just replace it with 'more than'
		// simultaneously we can discard result of comparing 4th and avoid correcting integer mask
		__m128 cmp0 = _mm_add_ps( max_dot_c3, _mm_and_ps( max_dots, dot_addition_mask ) );
		__m128 cmp1 = _mm_sub_ps( max_dot_c3, min_dots );
		
		__m128 comb = _mm_and_ps( cmp0, cmp1 );

		if ( !_mm_testc_si128( _mm_castps_si128( comb ), dot_compare_to_zero_mask ) )
			return intersect;
		else
			return inside;
	}
}