#ifndef __render_scene_h_included_
#define __render_scene_h_included_

#include <types.h>
#include "dx_include.h"

#include <math/math_3d.h>

#include <lib/array.h>
#include <lib/pool.h>

#include <game/camera.h>
#include <game/bvh.h>

#include "object.h"

#include "render_object_mesh.h"

namespace render {

class scene
{
public:
	void create( );
	void destroy( );

	void remove_all_static_objects( );
	void insert_static_object( object* in_object );
	void build_static_scene( );

	void dispatch_render_objects( frustum_aligned const& in_frustum );

protected:
	struct insert_render_object_helper;

	enum {
		static_render_objects_mesh_memory_ptr	= 0,
		static_render_objects_mesh_count		= 1024,
		static_render_objects_mesh_memory_size	= static_render_objects_mesh_count * sizeof(render_object_mesh*),

		//static_render_objects_light_memory_ptr	= static_render_objects_mesh_memory_ptr + static_render_objects_mesh_memory_size,
		//static_render_objects_light_count		= 256,
		//static_render_objects_light_memory_size	= static_render_objects_light_count * sizeof(render_object_light*),

		static_objects_memory_size				= static_render_objects_mesh_memory_size + static_render_objects_mesh_memory_size
	};
	
	typedef buffer_array<
		render_object_mesh*> static_render_objects_mesh_array;
	//typedef buffer_array<
	//	render_object_light> static_render_objects_light_array;

	typedef dynamic_allocation_pool<static_bvh_node_size::value, Memory_Page_Size, 256>
		bvh_node_pool;

	typedef static_bvh<render_object_mesh>
		static_render_objects_mesh;
	//typedef static_bvh<render_object_light>
	//	static_render_objects_light;
	
	pointer								m_static_objects_memory;
	static_render_objects_mesh_array	m_static_render_objects_mesh_array;
	//static_render_objects_light_array	m_static_render_objects_light_array;

	bvh_node_pool						m_bvh_node_allocator;

	static_render_objects_mesh			m_static_render_objects_mesh;
	//static_render_objects_light			m_static_render_objects_light;
};

} // namespace render

#endif // #ifndef __render_scene_h_included_