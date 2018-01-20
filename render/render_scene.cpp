#include "render_scene.h"
#include "render_parameters.h"

namespace render {

void scene::create( )
{
	math::u16x2 const screen_resolution = parameters::get_resolution( );
	float const aspect = (float)screen_resolution.x / (float)screen_resolution.y;

	m_camera.set_view_projection( math::float4x4::get_identity( ), c_default_fov, aspect, c_default_near_plane, c_default_far_plane );

	m_static_objects_memory = (pbyte)aligned_mem_allocator<64>::allocate( static_objects_memory_size );

	m_static_render_objects_mesh.set( m_static_objects_memory + static_render_objects_mesh_memory_ptr, static_render_objects_mesh_count );
	m_static_game_objects.set		( m_static_objects_memory + static_game_objects_memory_ptr, static_game_objects_count );
}

void scene::destroy( )
{
	m_static_game_objects.for_each( []( game_object* obj )
	{
		obj->destroy( );
	} );

	aligned_mem_allocator<64>::deallocate( m_static_objects_memory );
}

void scene::begin_static_building( )
{
	m_static_building = true;
}

void scene::end_static_building( )
{
	m_static_building = false;
}

render_object_mesh* scene::insert_render_object_mesh( )
{
	if ( m_static_building )
		return m_static_render_objects_mesh.emplace_back( );
	//else
}

void scene::remove_render_object_mesh( render_object_mesh* obj )
{
	// Assume object is dynamic
	//
}

void scene::dispatch( ) const
{
	m_static_game_objects.for_each( []( game_object const* obj )
	{
		obj->dispatch( );
	} );
}

} // namespace render