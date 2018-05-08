#ifndef __render_render_scene_h_included_
#define __render_render_scene_h_included_

#include <types.h>
#include <core/math.h>
#include <core/std.h>
#include <core/game.h>
#include "dx_include.h"

#include "game_object.h"

#include "render_object_mesh.h"

namespace render {

static const float c_default_fov		= math::degree_to_radian( 65.0f );
static const float c_default_near_plane	= 0.05f;
static const float c_default_far_plane	= 1000.0f;

class render_scene
{
public:
	void create( );
	void destroy( );

	void rebuild_static( );
	
	void clear_static_objects( );
	void insert_static_game_object( game_object* in_object );

	void dispatch( );

	inline camera& get_camera( ) { return m_camera; }

protected:
	struct insert_render_object
	{
		template<typename T>
		void call( T* in_object, render_scene* in_scene );
	};

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

	typedef dynamic_allocation_pool<static_bvh_node_size::value, 4096, 256>
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


	camera								m_camera;
};

} // namespace render

#endif // #ifndef __render_render_scene_h_included_