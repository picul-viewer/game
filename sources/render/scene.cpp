#include "scene.h"

#include "parameters.h"
#include "resources.h"

#include "object.h"

namespace render {

void scene::destroy_resource( scene* const in_resource )
{
	in_resource->m_static_mesh_container.for_each( []( math::bvh::object_handle const in_handle )
	{
		render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
		obj->destroy( );
	} );

	g_resources.render_allocator( ).deallocate( in_resource->m_container_memory );
	g_resources.render_allocator( ).deallocate( in_resource->m_static_objects_memory );
}

void scene::insert( object const& in_object )
{
	in_object.for_each_mesh_object( [this]( render_object_mesh* const in_obj )
	{
		math::bvh::object_handle const object_handle =
			(math::bvh::object_handle)g_resources.render_allocator( ).offset( in_obj );
		math::bvh::node_handle const node_handle =
			m_dynamic_mesh_container.insert( object_handle, in_obj->get_aabb( ).get_box( ) );
		in_obj->set_node_handle( node_handle );
	} );
}

void scene::remove( object const& in_object )
{
	in_object.for_each_mesh_object( [this]( render_object_mesh* const in_obj )
	{
		m_dynamic_mesh_container.remove( in_obj->get_node_handle( ) );
	} );
}

void scene::move( object const& in_object )
{
	in_object.for_each_mesh_object( [this]( render_object_mesh* const in_obj )
	{
		m_dynamic_mesh_container.move( in_obj->get_node_handle( ), in_obj->get_aabb( ).get_box( ) );
	} );
}

} // namespace render