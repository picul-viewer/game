#ifndef GUARD_RENDER_SCENE_H_INCLUDED
#define GUARD_RENDER_SCENE_H_INCLUDED

#include <types.h>

#include <math/bvh.h>
#include <math/math_3d.h>
#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>

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

	math::bvh const& static_mesh_container( ) const { return m_static_mesh_container; }

	math::bvh const& dynamic_mesh_container( ) const { return m_dynamic_mesh_container; }

private:
	pointer m_container_memory;
	pointer m_static_objects_memory;

	math::bvh m_static_mesh_container;
	math::bvh m_dynamic_mesh_container;
};

} // namespace render

#endif // #ifndef GUARD_RENDER_SCENE_H_INCLUDED