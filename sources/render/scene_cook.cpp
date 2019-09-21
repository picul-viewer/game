#include "scene_cook.h"
#include <lib/allocator.h>
#include "resources.h"

namespace render {

scene_cook* scene_cook::create( scene* const in_resource_ptr, lib::reader const& in_config )
{
	scene_cook* const result = std_allocator( ).allocate( sizeof(scene_cook) );

	result->init( );

	result->m_result = in_resource_ptr;
	result->m_config = in_config;

	return result;
}

void scene_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void scene_cook::create_resource( )
{
	u32 const mesh_max_count = 4096;

	u32 const static_mesh_count = m_config.read<u32>( );

	m_static_mesh_count = static_mesh_count;

	ASSERT_CMP( m_static_mesh_count, <=, mesh_max_count );

	uptr const static_mesh_bvh_memory_size = math::bvh::node_data_size * 2 * static_mesh_count;
	uptr const dynamic_mesh_bvh_memory_size = math::bvh::node_data_size * 2 * ( mesh_max_count - static_mesh_count );

	uptr const memory_size =
		static_mesh_bvh_memory_size +
		dynamic_mesh_bvh_memory_size;

	pointer const memory = virtual_allocator( ).allocate( memory_size );
	m_result->m_memory = memory;

	pointer ptr = memory;

	uptr const max_queries = 32768;
	lib::buffer_array<task_info> queries;
	queries.create( stack_allocate( max_queries * sizeof(task_info) ), max_queries );

	{
		m_static_mesh_handles = std_allocator( ).allocate( m_static_mesh_count * sizeof(math::bvh::object_handle) );

		for ( u32 i = 0; i < m_static_mesh_count; ++i )
		{
			render_object_mesh* const obj = g_resources.get_render_object_allocator( ).mesh_allocator( ).allocate( );
			new ( obj ) render_object_mesh( );
			obj->initialize( m_config, queries );
			m_static_mesh_handles[i] = (math::bvh::object_handle)g_resources.get_render_object_allocator( ).mesh_allocator( ).index_of( obj );
		}

		m_result->m_static_mesh_container.create( ptr, static_mesh_bvh_memory_size );
		m_result->m_static_mesh_container.deserialize( m_config, m_static_mesh_handles );
		ptr += static_mesh_bvh_memory_size;

		m_result->m_dynamic_mesh_container.create( ptr, dynamic_mesh_bvh_memory_size );
		ptr += dynamic_mesh_bvh_memory_size;
	}

	if ( queries.size( ) != 0 )
	{
		create_child_resources(
			queries.data( ), (u32)queries.size( ),
			callback_task<&scene_cook::on_child_resources_ready>( )
		);
	}
	else
		on_child_resources_ready( queried_resources( ) );
}

void scene_cook::on_child_resources_ready( queried_resources& in_queried )
{
	for ( u32 i = 0; i < m_static_mesh_count; ++i )
	{
		render_object_mesh* const obj = g_resources.get_render_object_allocator( ).mesh_allocator( )[m_static_mesh_handles[i]];
		obj->set_queried_resources( in_queried );
		obj->update( math::float4x3::identity( ) );
	}

	std_allocator( ).deallocate( m_static_mesh_handles );

	finish( m_result );
}

} // namespace render