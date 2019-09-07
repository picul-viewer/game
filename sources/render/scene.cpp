#include "scene.h"

#include "parameters.h"
#include "resources.h"

#include "object.h"

namespace render {

void scene::destroy_resource( scene* const in_resource )
{
	in_resource->m_dynamic_mesh_container.for_each( []( math::bvh::object_handle const in_handle )
	{
		render_object_mesh* const obj = g_resources.get_render_object_allocator( ).mesh_allocator( )[in_handle];
		obj->destroy( );
		g_resources.get_render_object_allocator( ).mesh_allocator( ).deallocate( obj );
	} );

	in_resource->m_static_mesh_container.for_each( []( math::bvh::object_handle const in_handle )
	{
		render_object_mesh* const obj = g_resources.get_render_object_allocator( ).mesh_allocator( )[in_handle];
		obj->destroy( );
		g_resources.get_render_object_allocator( ).mesh_allocator( ).deallocate( obj );
	} );

	virtual_allocator( ).deallocate( in_resource->m_memory );
}

struct scene::helper
{

struct functor_insert
{
	scene* scene_ptr;

	template<typename T>
	void operator( )( T* in_object ) const;

	template<>
	void operator( )( render_object_mesh* const in_object ) const
	{
		math::bvh::object_handle const object_handle =
			(math::bvh::object_handle)g_resources.get_render_object_allocator( ).mesh_allocator( ).index_of( in_object );
		math::bvh::node_handle const node_handle =
			scene_ptr->m_dynamic_mesh_container.insert( object_handle, in_object->get_aabb( ).get_box( ) );
		in_object->set_node_handle( node_handle );
	}
};

struct functor_remove
{
	scene* scene_ptr;

	template<typename T>
	void operator( )( T* in_object ) const;

	template<>
	void operator( )( render_object_mesh* const in_object ) const
	{
		scene_ptr->m_dynamic_mesh_container.remove( in_object->get_node_handle( ) );
	}
};

struct functor_move
{
	scene* scene_ptr;

	template<typename T>
	void operator( )( T* in_object ) const;

	template<>
	void operator( )( render_object_mesh* const in_object ) const
	{
		scene_ptr->m_dynamic_mesh_container.move( in_object->get_node_handle( ), in_object->get_aabb( ).get_box( ) );
	}
};

};

void scene::insert( object const& in_object )
{
	g_resources.get_render_object_allocator( ).for_each( in_object.objects( ), helper::functor_insert{ this } );
}

void scene::remove( object const& in_object )
{
	g_resources.get_render_object_allocator( ).for_each( in_object.objects( ), helper::functor_remove{ this } );
}

void scene::move( object const& in_object )
{
	g_resources.get_render_object_allocator( ).for_each( in_object.objects( ), helper::functor_move{ this } );
}

} // namespace render