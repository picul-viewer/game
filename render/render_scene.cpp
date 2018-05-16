#include "render_scene.h"

#include "render_parameters.h"
#include "render_resources.h"

namespace render {

void render_scene::create( )
{
	math::u16x2 const screen_resolution = g_parameters->screen_resolution;
	float const aspect = (float)screen_resolution.x / (float)screen_resolution.y;

	m_camera.set_view_projection( math::float4x4::get_identity( ), c_default_fov, aspect, c_default_near_plane, c_default_far_plane );

	m_static_objects_memory = aligned_mem_allocator<64>( ).allocate( static_objects_memory_size );

	m_static_render_objects_mesh_array.set	( m_static_objects_memory + (uptr)static_render_objects_mesh_memory_ptr, static_render_objects_mesh_count );
	//m_static_render_objects_light_array.set	( m_static_objects_memory + static_render_objects_light_memory_ptr, static_render_objects_light_count );
}

void render_scene::destroy( )
{
	m_static_render_objects_mesh.destroy( );
	//m_static_render_objects_light.destroy( );

	aligned_mem_allocator<64>( ).deallocate( m_static_objects_memory );
}

void render_scene::rebuild_static( )
{
	m_static_render_objects_mesh.destroy( );

	m_static_render_objects_mesh.create( m_bvh_node_allocator, m_static_render_objects_mesh_array );
}

void render_scene::clear_static_objects( )
{
	m_static_render_objects_mesh_array.clear( );
}

template<typename T>
void render_scene::insert_render_object::call( T* in_object, render_scene* in_scene )
{
	FATAL_ERROR( "incorrect render object type" );
}

template<>
void render_scene::insert_render_object::call( render_object_mesh* in_object, render_scene* in_scene )
{
	in_scene->m_static_render_objects_mesh_array.push_back( in_object );
}

void render_scene::insert_static_game_object( game_object* in_object )
{
	in_object->for_each( [this]( render_object* current )
	{
		resources::get_render_objects_allocator( ).execute_typed( current, insert_render_object( ), this );
	} );
}

void render_scene::dispatch( )
{
	mem_align(16)
	frustum_aligned frustum;

	frustum.set_from_matrix( m_camera.get_view( ) );

	m_static_render_objects_mesh.query_visibility( frustum, []( render_object_mesh* current ) { current->dispatch( ); } );
}

} // namespace render