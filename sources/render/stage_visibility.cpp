#include "stage_visibility.h"
#include "renderer.h"
#include "resources.h"
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
	auto const& static_mesh_container = g_renderer.m_scene->static_mesh_container( );
	auto const& dynamic_mesh_container = g_renderer.m_scene->dynamic_mesh_container( );
	
	// Initialize render data.
	math::frustum frustum;
	frustum.set_from_matrix( g_renderer.m_render_camera.get_view_projection( ) );
	
	g_renderer.m_render_meshes.clear( );

	static_mesh_container.frustum_test( frustum, []( math::bvh::object_handle const in_handle )
	{
		render_object_mesh* const object = g_resources.get_render_object_allocator( ).mesh_allocator( )[in_handle];
		g_renderer.m_render_meshes.push_back( object );
	} );

	dynamic_mesh_container.frustum_test( frustum, []( math::bvh::object_handle const in_handle )
	{
		render_object_mesh* const object = g_resources.get_render_object_allocator( ).mesh_allocator( )[in_handle];
		g_renderer.m_render_meshes.push_back( object );
	} );
}

} // namespace render