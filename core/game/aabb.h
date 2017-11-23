#ifndef __core_aabb_h_included_
#define __core_aabb_h_included_

#include <core/macros.h>
#include <core/math.h>

struct aabb
{
public:
	inline void set_min_max( math::float3 const& min, math::float3 const& max );
	inline void set_center_radius( math::float3 const& center, math::float3 const& radius );
	
	inline math::float3 center( ) const;
	inline math::float3 radius( ) const;
	
	inline	void modify( math::float3 const& transform );
			void modify( math::float3x3 const& transform );
			void modify( math::float4x3 const& transform );
	
public:
	math::float3 min;
	math::float3 max;
};

mem_align(16)
struct aabb_aligned
{
public:
	inline aabb_aligned( );
	
	inline void set_min_max( math::float4 const& min, math::float4 const& max );
	inline void set_center_radius( math::float4 const& center, math::float4 const& radius );
	
	inline void set_min_max( __m128 const& min, __m128 const& max );
	inline void set_center_radius( __m128 const& center, __m128 const& radius );
	
	inline math::float4 center( ) const;
	inline math::float4 radius( ) const;
	
	inline void get_center_radius( math::float4& center, math::float4& radius );
	inline void get_center_radius( __m128& center, __m128& radius );
	
	inline	void modify( math::float4 const& transform );
			void modify( math::float4x3 const& transform );
	
public:
	math::float3 min;
	
protected:
	// Always equals 1.0f - for simple transpose
	// extraction when modifying box.
	float unused0;
	
public:
	math::float3 max;
	
protected:
	// Always equals 1.0f - for simple transpose
	// extraction when modifying box.
	float unused1;
};

#include "aabb_inline.h"

#endif // #ifndef __core_aabb_h_included_