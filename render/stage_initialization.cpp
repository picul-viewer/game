#include "renderer.h"
#include "scene.h"
#include "resources.h"

#include <game/camera.h>

namespace render {

void stage_initialization::create( )
{ }

void stage_initialization::destroy( )
{ }

void stage_initialization::execute( )
{
	// Initialize renderer
	g_resources.get_backbuffer( ).clear( math::float4( 0.0f ) );
	g_resources.get_depth_buffer( ).get_dsv( ).clear( 0.0f, 0u );

	// Process scene objects
	auto static_render_object_mesh_container = g_renderer.m_scene->get_static_render_objects_mesh_container( );

	frustum_aligned f;
	f.set_from_matrix( g_renderer.m_camera->get_view_projection( ) );

	static_render_object_mesh_container.query_visibility( f, []( render_object_mesh* in_object )
	{
		g_renderer.m_render_meshes.push_back( in_object );
	} );

	g_resources.bind_default_resources( );
}

} // namespace render