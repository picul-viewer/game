#include "renderer.h"
#include "scene.h"
#include "resources.h"
#include "parameters.h"

#include <math/frustum.h>

namespace render {

void stage_initialization::create( )
{ }

void stage_initialization::destroy( )
{ }

void stage_initialization::execute( )
{
	// Initialize renderer
	g_resources.get_backbuffer( ).clear( math::float4( 0.0f ) );
	g_resources.get_depth_buffer( ).get_dsv( ).clear( 1.0f, 0u );

	// Process scene objects
	auto static_render_object_mesh_container = g_renderer.m_scene->get_static_render_objects_mesh_container( );
	
	// Initialize render camera
	g_renderer.m_render_camera.set_view( g_parameters.camera.view );
	g_renderer.m_render_camera.set_perspective( g_parameters.camera.fov, 1.3333f, 0.01f, 100.0f );
	g_renderer.m_render_camera.update( );

	// Initialize render data
	math::frustum_aligned frustum;
	frustum.set_from_matrix( g_renderer.m_render_camera.get_view_projection( ) );
	
	g_renderer.m_render_meshes.clear( );
	static_render_object_mesh_container.query_visibility( frustum, []( render_object_mesh* in_object )
	{
		g_renderer.m_render_meshes.push_back( in_object );
	} );

	g_resources.bind_default_resources( );
}

} // namespace render