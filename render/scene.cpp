#include "scene.h"

#include "render_parameters.h"
#include "resources.h"

namespace render {

void scene::create( )
{
	m_bvh_node_allocator.create( );

	m_static_objects_memory = virtual_mem_allocator( ).allocate( static_objects_memory_size );

	m_static_render_objects_mesh_array.set	( m_static_objects_memory + (uptr)static_render_objects_mesh_memory_ptr, static_render_objects_mesh_count );
	//m_static_render_objects_light_array.set	( m_static_objects_memory + static_render_objects_light_memory_ptr, static_render_objects_light_count );
}

void scene::destroy( )
{
	m_static_render_objects_mesh_container.destroy( );
	//m_static_render_objects_light_container.destroy( );

	virtual_mem_allocator( ).deallocate( m_static_objects_memory );
	
	m_static_render_objects_mesh_container.destroy( );

	m_bvh_node_allocator.destroy( );
}

struct scene::insert_render_object_helper
{
	void operator( )( render_object_mesh* in_object, scene* in_scene )
	{
		in_scene->m_static_render_objects_mesh_array.push_back( in_object );
	}
};

void scene::remove_all_static_objects( )
{
	m_static_render_objects_mesh_array.clear( );
}

void scene::insert_static_object( object* in_object )
{
	in_object->for_each( [this]( render_object* current )
	{
		g_resources.get_render_object_allocator( ).execute( current, insert_render_object_helper( ), this );
	} );
}

void scene::build_static_scene( )
{
	m_static_render_objects_mesh_container.destroy( );
	m_static_render_objects_mesh_container.create( m_bvh_node_allocator, m_static_render_objects_mesh_array );
}

} // namespace render