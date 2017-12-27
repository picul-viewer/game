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

#endif // #ifndef __core_aabb_inline_h_included_