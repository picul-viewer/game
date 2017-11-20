#ifndef __render_render_scene_h_included_
#define __render_render_scene_h_included_

#include <core/types.h>
#include <core/math.h>
#include <core/structs.h>
#include <core/game.h>
#include "dx_include.h"

#include "game_object.h"

#include "render_object_mesh.h"

namespace render {

static const float c_default_fov		= math::degree_to_radian( 65.0f );
static const float c_default_near_plane	= 0.05f;
static const float c_default_far_plane	= 1000.0f;

class scene
{
public:
	void create( );
	void destroy( );

	void begin_static_building( );
	void end_static_building( );

	render_object_mesh* insert_render_object_mesh( );
	void remove_render_object_mesh( render_object_mesh* obj );

	void dispatch( ) const;

	inline camera& get_camera( ) { return m_camera; }

protected:
	enum {
		static_render_objects_mesh_memory_ptr	= 0,
		static_render_objects_mesh_count		= 1024,
		static_render_objects_mesh_memory_size	= static_render_objects_mesh_count * sizeof(render_object_mesh),

		static_game_objects_memory_ptr			= static_render_objects_mesh_memory_ptr + static_render_objects_mesh_memory_size,
		static_game_objects_count				= 256,
		static_render_objects_mesh_memory_size	= static_render_objects_mesh_count * sizeof(render_object_mesh),

		static_objects_memory_size				= static_game_objects_memory_ptr + static_render_objects_mesh_memory_size
	};

	typedef buffer_array<
		render_object_mesh> static_render_objects_mesh_array;

	typedef buffer_array<
		game_object> static_game_objects_array;

	pbyte								m_static_objects_memory;

	static_render_objects_mesh_array	m_static_render_objects_mesh;

	static_game_objects_array			m_static_game_objects;


	camera								m_camera;

	bool								m_static_building;
};

} // namespace render

#endif // #ifndef __render_render_scene_h_included_