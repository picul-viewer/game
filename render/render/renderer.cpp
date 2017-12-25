#include "renderer.h"

#include <core/std.h>

#include "render_api.h"

#include "resource_views.h"
#include "render_targets.h"

#include "render_parameters.h"

#include "render_allocators.h"
#include "render_scene.h"

#include "stages.h"

namespace render {

void renderer_data::create( )
{
	m_rendered_meshes.set( (render_object_mesh**)( m_rendered_objects_memory.data( ) + rendered_meshes_memory_offset ), max_rendered_meshes );

	//////////////////////

	{
		render_target_view::cook cook;
		cook.set_tex2d_rtv( api::backbuffer_pixel_format );
		m_backbuffer_rt.create( api::get_backbuffer( ), cook );
	}

	m_depth_buffer.create( DXGI_FORMAT_D24_UNORM_S8_UINT, parameters::get_resolution( ), D3D11_BIND_DEPTH_STENCIL );
}

void renderer_data::destroy( )
{
	m_backbuffer_rt.destroy( );
	m_depth_buffer.destroy( );
}

namespace renderer
{

extern renderer_data			g_data;

extern render_objects_allocator	g_render_objects_allocator;
extern render_scene				g_scene;

void create( )
{
	g_data.create( );
	g_scene.create( );
}

void destroy( )
{
	g_data.destroy( );
	g_scene.destroy( );
}

void render( )
{
	stage_prepare_objects::execute	( );
	stage_forward_default::execute	( );

	render_core::end_frame			( );
}

} // namespace renderer

} // namespace render