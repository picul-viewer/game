#ifndef GUARD_CORE_AABB_H_INCLUDED
#define GUARD_CORE_AABB_H_INCLUDED

#include <types.h>

#include "vector.h"
#include "matrix.h"
#include "math_sse.h"

namespace math {

struct aabb
{
public:
	aabb get_box( ) const;

	void set_min_max( float3 const& min, float3 const& max );
	void set_center_radius( float3 const& center, float3 const& radius );
	
	float3 center( ) const;
	float3 radius( ) const;
	
	float3 get_min( ) const;
	float3 get_max( ) const;

	void extrude( float3 const& point );
	void extrude( aabb const& box );

	void modify( float3 const& transform );
	void modify( float3x3 const& transform );
	void modify( float4x3 const& transform );
	
	float surface_area( ) const;
public:
	float3 m_min;
	float3 m_max;
};

mem_align(16)
struct aabb_aligned
{
public:
	aabb_aligned( );
	
	aabb get_box( ) const;

	void set_min_max( sse::vector const& min, sse::vector const& max );
	void set_center_radius( sse::vector const& center, sse::vector const& radius );
	
	sse::vector center( ) const;
	sse::vector radius( ) const;
	
	void get_center_radius( sse::vector& center, sse::vector& radius );
	
	float3 get_min( ) const;
	float3 get_max( ) const;

	void extrude( sse::vector const& point );
	void extrude( aabb_aligned const& box );

	void modify( sse::vector const& transform );
	void modify( sse::matrix3 const& transform );

	float surface_area( ) const;
public:
	sse::vector m_min;
	sse::vector m_max;
};

} // namespace math

#endif // #ifndef GUARD_CORE_AABB_H_INCLUDED