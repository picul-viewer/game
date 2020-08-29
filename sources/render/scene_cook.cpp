#include "scene_cook.h"
#include <lib/allocator.h>
#include "resources.h"
#include "scene.h"
#include "render_object_mesh.h"
#include "render_object_point_light.h"

namespace render {

scene_cook::scene_cook( scene* const in_resource_ptr, lib::reader const& in_config ) :
	m_result( in_resource_ptr ),
	m_config( in_config )
{ }

void scene_cook::create_resource( )
{
	u32 const mesh_max_count = 16384;
	u32 const max_light_count = 4096;

	u32 const static_mesh_count = m_config.read<u32>( );
	ASSERT_CMP( static_mesh_count, <=, mesh_max_count );

	u32 const static_point_light_count = m_config.read<u32>( );
	ASSERT_CMP( static_point_light_count, <=, max_light_count );

	m_static_mesh_count = static_mesh_count;

	uptr const objects_allocation_size =
		sizeof(render_object_mesh) * static_mesh_count +
		sizeof(render_object_point_light) * static_point_light_count;
	m_result->m_static_objects_memory = g_resources.render_allocator( ).allocate( objects_allocation_size );

	uptr const static_mesh_bvh_memory_size = math::bvh::node_data_size * 2 * static_mesh_count;
	uptr const dynamic_mesh_bvh_memory_size = math::bvh::node_data_size * 2 * ( mesh_max_count - static_mesh_count );

	uptr const memory_size =
		static_mesh_bvh_memory_size +
		dynamic_mesh_bvh_memory_size;

	pointer const memory = g_resources.render_allocator( ).allocate( memory_size );
	m_result->m_container_memory = memory;

	pointer ptr = memory;

	uptr const max_queries = 32768;
	lib::buffer_array<task_info> queries;
	queries.create( stack_allocate( max_queries * sizeof(task_info) ), max_queries );

	{
		math::bvh::object_handle* const handles = stack_allocate( static_mesh_count * sizeof(math::bvh::object_handle) );
		lib::buffer_array<render_object_mesh> static_meshes( m_result->m_static_objects_memory, static_mesh_count, static_mesh_count );

		for ( u32 i = 0; i < static_mesh_count; ++i )
		{
			new ( &static_meshes[i] ) render_object_mesh( );
			static_meshes[i].create( m_config, queries );
			handles[i] = (math::bvh::object_handle)g_resources.render_allocator( ).offset( &static_meshes[i] );
		}

		m_result->m_static_mesh_container.create( ptr, static_mesh_bvh_memory_size );
		m_result->m_static_mesh_container.deserialize( m_config, handles );
		ptr += static_mesh_bvh_memory_size;


		m_result->m_static_point_light_container.create( (render_object_point_light*)static_meshes.end( ), static_point_light_count, static_point_light_count );
		lib::buffer_array<render_object_point_light>& static_point_lights = m_result->m_static_point_light_container;

		for ( u32 i = 0; i < static_point_lights.size( ); ++i )
		{
			new ( &static_point_lights[i] ) render_object_point_light( );
			static_point_lights[i].create( m_config, queries );
		}

		m_result->m_sun_direction = m_config.read<math::float3>( );
		m_result->m_sun_radiance = m_config.read<math::float3>( );
	}

	{
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
	u32 const max_task_count = 16384;
	lib::buffer_array<gpu_upload_task> tasks;
	tasks.create( stack_allocate( sizeof(gpu_upload_task) * max_task_count ), max_task_count );

	uptr const allocator_size = 64 * Kb;
	linear_allocator allocator;
	allocator.create( stack_allocate( allocator_size ), allocator_size );

	render_object_mesh* obj_mesh = m_result->m_static_objects_memory;

	for ( u32 i = 0; i < m_static_mesh_count; ++i, ++obj_mesh )
		obj_mesh->on_resources_ready( in_queried, tasks, allocator, true );

	lib::buffer_array<render_object_point_light>& static_point_lights = m_result->m_static_point_light_container;
	for ( u32 i = 0; i < static_point_lights.size( ); ++i )
		static_point_lights[i].on_resources_ready( in_queried, tasks, allocator, true );

	if ( tasks.size( ) == 0 )
		finish( m_result );
	else
	{
		custom_task_context context = create_child_custom_tasks(
			(u32)tasks.size( ),
			resource_system::user_callback_task<scene_cook, &scene_cook::on_copy_tasks_finished>( this )
		);

		lib::for_each( tasks.begin( ), tasks.end( ), [context]( gpu_upload_task& in_task )
		{
			in_task.set_task_context( context );
		} );

		g_gpu_uploader.push_background_tasks( tasks.data( ), (u32)tasks.size( ) );
	}
}

void scene_cook::on_copy_tasks_finished( )
{
	finish( m_result );
}

} // namespace render