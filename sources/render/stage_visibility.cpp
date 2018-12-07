#include "stage_visibility.h"
#include "renderer.h"
#include "scene.h"

#include <math/frustum.h>

namespace render {

void stage_visibility::create( )
{ }

void stage_visibility::destroy( )
{ }

void stage_visibility::execute( )
{
	// Process scene objects.
	auto const& static_render_object_mesh_container = g_renderer.m_scene->get_static_render_objects_mesh_container( );
	
	// Initialize render data.
	math::frustum_aligned frustum;
	frustum.set_from_matrix( g_renderer.m_render_camera.get_view_projection( ) );
	
	g_renderer.m_render_meshes.clear( );
	static_render_object_mesh_container.query_visibility( frustum, []( render_object_mesh* in_object )
	{
		g_renderer.m_render_meshes.push_back( in_object );
	} );
}

} // namespace render