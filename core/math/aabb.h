#ifndef __core_aabb_h_included_
#define __core_aabb_h_included_

#include <types.h>

#include "vector.h"
#include "matrix.h"
#include "math_sse.h"

struct aabb
{
public:
	aabb& get_box( );
	aabb const& get_box( ) const;

	void set_min_max( math::float3 const& min, math::float3 const& max );
	void set_center_radius( math::float3 const& center, math::float3 const& radius );
	
	math::float3 center( ) const;
	math::float3 radius( ) const;
	
	void extrude( math::float3 const& point );
	void extrude( aabb const& box );

	void modify( math::float3 const& transform );
	void modify( math::float3x3 const& transform );
	void modify( math::float4x3 const& transform );
	
	float surface_area( ) const;
public:
	math::float3 min;
	math::float3 max;
};

mem_align(16)
struct aabb_aligned
{
public:
	aabb_aligned( );
	
	aabb& get_box( );
	aabb const& get_box( ) const;

	void set_min_max( math::sse::vector const& min, math::sse::vector const& max );
	void set_center_radius( math::sse::vector const& center, math::sse::vector const& radius );
	
	math::sse::vector center( ) const;
	math::sse::vector radius( ) const;
	
	void get_center_radius( math::sse::vector& center, math::sse::vector& radius );
	
	void extrude( math::sse::vector const& point );
	void extrude( aabb_aligned const& box );

	void modify( math::sse::vector const& transform );
	void modify( math::sse::matrix3 const& transform );

	float surface_area( ) const;
public:
	math::sse::vector min;
	math::sse::vector max;
};

#endif // #ifndef __core_aabb_h_included_