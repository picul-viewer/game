#ifndef __core_frustum_h_included_
#define __core_frustum_h_included_

#include <core/types.h>
#include <core/math.h>

#include "aabb.h"

enum intersection
{
	inside = 0,
	intersect,
	outside
};

class frustum
{
public:
	void set_from_matrix( math::float4x4 const& m );

	bool test_aabb_inside( aabb const& box );
	bool test_aabb_ouside( aabb const& box );

	intersection test_aabb( aabb const& box );

protected:
	struct plane
	{
		union
		{
			math::float4 vector;
			struct
			{
				math::float3 n;
				float d;
			};
		};

		plane( );

		bool test_aabb_inside( aabb const& box );
		bool test_aabb_ouside( aabb const& box );

		intersection test_aabb( aabb const& box );
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

	plane planes[6];
};

mem_align(16)
class frustum_aligned
{
public:
	frustum_aligned( );

	void set_from_matrix( math::float4x4 const& m );

	bool test_aabb_inside( aabb_aligned const& box );
	bool test_aabb_ouside( aabb_aligned const& box );

	intersection test_aabb( aabb_aligned const& box );

protected:
	math::float4x4 matrix;
};

#endif // #ifndef __core_frustum_h_included_