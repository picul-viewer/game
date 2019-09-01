#include "object_cook.h"
#include <lib/allocator.h>
#include "resources.h"

#include "render_object_mesh.h"

namespace render {

object_cook* object_cook::create( object* const in_resource_ptr, lib::reader const& in_config )
{
	object_cook* const result = std_allocator( ).allocate( sizeof(object_cook) );

	result->init( );

	result->m_result = in_resource_ptr;
	result->m_config = in_config;

	return result;
}

void object_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void object_cook::create_resource( )
{
	u32 const max_queries = 4096;

	lib::buffer_array<task_info> queries;
	queries.create( stack_allocate( max_queries * sizeof(task_info) ), max_queries );

	render_object** obj_ptr = &m_result->m_objects;

	u16 const render_object_count = m_config.read<u16>( );
	ASSERT( render_object_count != 0 );

	u16 i = 0;

	for ( ; i < render_object_count; ++i )
	{
		u8 const object_type = m_config.read<u8>( );
		
		ASSERT( object_type == render_object_type_mesh );

		render_object_mesh* obj	= g_resources.get_render_object_allocator( ).mesh_allocator( ).allocate( );
		new ( obj ) render_object_mesh( );
		obj->initialize( m_config, queries );

		*obj_ptr = obj;
		obj_ptr = &obj->m_next;
	}

	*obj_ptr = nullptr;

	if ( queries.size( ) != 0 )
	{
		create_child_resources(
			queries.data( ), (u32)queries.size( ),
			callback_task<&object_cook::on_child_resources_ready>( )
		);
	}
	else
		on_child_resources_ready( queried_resources( ) );
}

struct functor_set_queried_resources
{
	queried_resources& resources;

	template<typename T>
	void operator( )( T* const in_object ) const
	{
		in_object->set_queried_resources( resources );
	}
};

void object_cook::on_child_resources_ready( queried_resources& in_queried )
{
	g_resources.get_render_object_allocator( ).for_each( m_result->m_objects, functor_set_queried_resources{ in_queried } );

	finish( m_result );
}

} // namespace render