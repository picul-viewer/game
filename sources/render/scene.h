#ifndef GUARD_RENDER_SCENE_H_INCLUDED
#define GUARD_RENDER_SCENE_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <math/bvh.h>
#include <math/math_3d.h>
#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>
#include "render_object_point_light.h"

namespace render {

class object;

class scene : public default_resource<scene>
{
public:
	friend class scene_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_helper_thread_0
	};

	static void destroy_resource( scene* const in_resource );

public:
	void insert( object const& in_object );
	void remove( object const& in_object );
	void move( object const& in_object );

	inline void set_sun_direction( math::float3 const& in_direction ) { m_sun_direction = in_direction; }
	inline void set_sun_radiance( math::float3 const& in_radiance ) { m_sun_radiance = in_radiance; }
	
	inline math::float3 get_sun_direction( ) const { return m_sun_direction; }
	inline math::float3 get_sun_radiance( ) const { return m_sun_radiance; }

	inline math::bvh const& static_mesh_container( ) const { return m_static_mesh_container; }
	inline math::bvh const& dynamic_mesh_container( ) const { return m_dynamic_mesh_container; }

	inline lib::buffer_array<render_object_point_light> const& static_point_light_container( ) const { return m_static_point_light_container; }

private:
	pointer m_container_memory;
	pointer m_static_objects_memory;

	math::bvh m_static_mesh_container;
	math::bvh m_dynamic_mesh_container;

	lib::buffer_array<render_object_point_light> m_static_point_light_container;

	math::float3 m_sun_direction;
	math::float3 m_sun_radiance;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_SCENE_H_INCLUDED