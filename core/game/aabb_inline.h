#ifndef __core_aabb_inline_h_included_
#define __core_aabb_inline_h_included_

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

void aabb_aligned::set_min_max( math::float4 const& min, math::float4 const& max )
{
	ASSERT( aligned( &min, 16 ) );
	ASSERT( aligned( &max, 16 ) );
	
	// unused variables should be equal to 1.0f
	ASSERT( min.w == 1.0f );
	ASSERT( max.w == 1.0f );

	_mm_store_ps( this->min.data, _mm_load_ps( min.data ) );
	_mm_store_ps( this->max.data, _mm_load_ps( max.data ) );
}

void aabb_aligned::set_center_radius( math::float4 const& center, math::float4 const& radius )
{
	ASSERT( aligned( &center, 16 ) );
	ASSERT( aligned( &radius, 16 ) );
	
	// unused variables should be equal to 1.0f
	ASSERT( center.w == 1.0f );
	ASSERT( radius.w == 0.0f );
	
	__m128 c = _mm_load_ps( center.data );
	__m128 r = _mm_load_ps( radius.data );

	_mm_store_ps( min.data, _mm_sub_ps( c, r ) );
	_mm_store_ps( max.data, _mm_add_ps( c, r ) );
}

math::float4 aabb_aligned::center( ) const
{
	__m128 l = _mm_load_ps( min.data );
	__m128 h = _mm_load_ps( max.data );

	__m128 sum = _mm_add_ps( l, h );
	__m128 c = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );

	mem_align(16) math::float4 res;
	_mm_store_ps( res.data, c );

	return res;
}

math::float4 aabb_aligned::radius( ) const
{
	__m128 l = _mm_load_ps( min.data );
	__m128 h = _mm_load_ps( max.data );

	__m128 dif = _mm_sub_ps( h, l );
	__m128 r = _mm_mul_ps( dif, _mm_set1_ps( 0.5f ) );

	mem_align(16) math::float4 res;
	_mm_store_ps( res.data, r );

	return res;
}

void aabb_aligned::get_center_radius( math::float4& center, math::float4& radius )
{
	ASSERT( aligned( &center, 16 ) );
	ASSERT( aligned( &radius, 16 ) );

	__m128 l = _mm_load_ps( min.data );
	__m128 h = _mm_load_ps( max.data );
	
	__m128 sum = _mm_add_ps( l, h );

	__m128 c = _mm_mul_ps( sum, _mm_set1_ps( 0.5f ) );
	__m128 r = _mm_sub_ps( h, c );

	_mm_store_ps( center.data, c );
	_mm_store_ps( radius.data, r );
}

void aabb_aligned::modify( math::float4 const& transform )
{
	ASSERT( aligned( &transform, 16 ) );

	// Do not modify unused variables
	ASSERT( transform.w == 0.0f );

	__m128 t = _mm_load_ps( transform.data );
	__m128 l = _mm_load_ps( min.data );
	__m128 h = _mm_load_ps( max.data );
	
	_mm_store_ps( min.data, _mm_add_ps( l, t ) );
	_mm_store_ps( max.data, _mm_add_ps( h, t ) );
}

#endif // #ifndef __core_aabb_inline_h_included_