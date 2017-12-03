#ifndef __core_aabb_h_included_
#define __core_aabb_h_included_

#include <core/macros.h>
#include <core/math.h>

struct aabb
{
public:
	inline aabb& get_box( );
	inline aabb const& get_box( ) const;

	inline void set_min_max( math::float3 const& min, math::float3 const& max );
	inline void set_center_radius( math::float3 const& center, math::float3 const& radius );
	
	inline math::float3 center( ) const;
	inline math::float3 radius( ) const;
	
	inline void extrude( math::float3 const& point );
	inline void extrude( aabb const& box );

	inline	void modify( math::float3 const& transform );
			void modify( math::float3x3 const& transform );
			void modify( math::float4x3 const& transform );
	
	inline float surface_area( ) const;
public:
	math::float3 min;
	math::float3 max;
};

mem_align(16)
struct aabb_aligned
{
public:
	inline aabb_aligned( );
	
	inline aabb& get_box( );
	inline aabb const& get_box( ) const;

	inline void set_min_max( math::sse::vector const& min, math::sse::vector const& max );
	inline void set_center_radius( math::sse::vector const& center, math::sse::vector const& radius );
	
	inline math::sse::vector center( ) const;
	inline math::sse::vector radius( ) const;
	
	inline void get_center_radius( math::sse::vector& center, math::sse::vector& radius );
	
	inline void extrude( math::sse::vector const& point );
	inline void extrude( aabb_aligned const& box );

	inline	void modify( math::sse::vector const& transform );
			void modify( math::sse::matrix3 const& transform );

	inline float surface_area( ) const;
public:
	math::sse::vector min;
	math::sse::vector max;
};

#include "aabb_inline.h"

#endif // #ifndef __core_aabb_h_included_