#ifndef __render_renderer_h_included_
#define __render_renderer_h_included_

#include <types.h>

#include <lib/buffer_array.h>

#include "stage_initialization.h"
#include "stage_forward_default.h"

#include "render_object_mesh.h"

#include "camera.h"

namespace render {

class scene;

class renderer
{
public:
	renderer( );

	void create( );
	void destroy( );
	
	inline void set_scene( scene* in_scene ) { m_next_scene = in_scene; }

	void render( );

protected:
	// Stages
	friend class stage_initialization;
	friend class stage_forward_default;
	
	void render_scene( );

	void end_frame( );

protected:
	enum
	{
		max_render_meshes	= 1024,
		//
	};

	enum
	{
		render_meshes_memory_offset = 0,
		//
		render_objects_memory_size = render_meshes_memory_offset + sizeof(render_object_mesh*) * max_render_meshes
	};
	
	typedef buffer_array<render_object_mesh const*> render_meshes_type;
	
protected:
	camera					m_render_camera;

	scene*					m_scene;
	scene*					m_next_scene;

	pointer					m_render_objects_memory;

	render_meshes_type		m_render_meshes;

	stage_initialization	m_stage_initialization;
	stage_forward_default	m_stage_forward_default;
};

extern renderer g_renderer;

} // namespace render

#endif // #ifndef __render_renderer_h_included_