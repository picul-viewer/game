#ifndef __render_renderer_h_included_
#define __render_renderer_h_included_

#include <core/types.h>
#include "dx_include.h"

#include "resource_views.h"
#include "render_targets.h"

#include "render_object.h"
#include "render_object_mesh.h"

#include "render_scene.h"

namespace render {

class renderer_data
{
public:
	typedef buffer_array<render_object_mesh const*> render_meshes_type;

public:
	void create( );
	void destroy( );

	inline render_meshes_type& get_render_meshes( ) { return m_render_meshes; }

	inline render_target_view& get_backbuffer( ) { return m_backbuffer_rt; }
	inline render_target_tex2d& get_depth_buffer( ) { return m_depth_buffer; }

protected:
	enum
	{
		max_rendered_meshes	= 1024,
		//
	};

	enum
	{
		rendered_meshes_memory_offset = 0,
		//
		rendered_objects_memory_size = rendered_meshes_memory_offset + sizeof(render_object_mesh*) * max_rendered_meshes
	};

	fixed_array<s8, rendered_objects_memory_size>	m_rendered_objects_memory;
	render_meshes_type								m_render_meshes;
	
	render_target_view								m_backbuffer_rt;
	render_target_tex2d								m_depth_buffer;
};

namespace renderer
{
	void create( );
	void destroy( );
	
	void render( );
	
} // namespace renderer

} // namespace render

#endif // #ifndef __render_renderer_h_included_