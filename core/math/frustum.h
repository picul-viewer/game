#ifndef __core_frustum_h_included_
#define __core_frustum_h_included_

#include <types.h>

#include "math_sse.h"
#include "aabb.h"

namespace math {

enum intersection
{
	inside = 0,
	intersect,
	outside
};

class frustum
{
public:
	frustum( ) = default;
	frustum( float4x4 const& m );

	void set_from_matrix( float4x4 const& m );

	bool test_aabb_inside( aabb const& box ) const;
	bool test_aabb_outside( aabb const& box ) const;

	intersection test_aabb( aabb const& box ) const;

protected:
	struct plane
	{
		union
		{
			float4 vector;
			struct
			{
				float3 n;
				float d;
			};
		};

		bool test_aabb_inside( aabb const& box ) const;
		bool test_aabb_outside( aabb const& box ) const;

		intersection test_aabb( aabb const& box ) const;
	};

	enum plane_id
	{
		left_plane = 0,
		right_plane,
		bottom_plane,
		top_plane,
		near_plane,
		far_plane,
		plane_count
	};

	plane planes[plane_count];
};

mem_align(16)
class frustum_aligned
{
public:
	frustum_aligned( ) = default;
	frustum_aligned( sse::matrix const& m );

	void set_from_matrix( sse::matrix const& m );

	bool test_aabb_inside( aabb_aligned const& box ) const;
	bool test_aabb_outside( aabb_aligned const& box ) const;

	intersection test_aabb( aabb_aligned const& box ) const;

protected:
	__m128 planes[8];
};

} // namespace math

#endif // #ifndef __core_frustum_h_included_