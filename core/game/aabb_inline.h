#ifndef __core_aabb_inline_h_included_
#define __core_aabb_inline_h_included_

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

void aabb::modify( math::float3 const& transform )
{
	min += transform;
	max += transform;
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
	ASSERT( min.data.m128_f32[3] == 1.0f );
	ASSERT( max.data.m128_f32[3] == 1.0f );

	this->min = min;
	this->max = max;
}

void aabb_aligned::set_center_radius( math::sse::vector const& center, math::sse::vector const& radius )
{
	ASSERT( center.data.m128_f32[3] == 1.0f );
	ASSERT( radius.data.m128_f32[3] == 0.0f );
	
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

void aabb_aligned::modify( math::sse::vector const& transform )
{
	// Do not modify unused variables
	ASSERT( transform.data.m128_f32[3] == 0.0f );

	min = _mm_add_ps( min, transform );
	max = _mm_add_ps( max, transform );
}

#endif // #ifndef __core_aabb_inline_h_included_