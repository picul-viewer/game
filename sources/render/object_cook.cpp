#include "object_cook.h"
#include <lib/allocator.h>

#include "object.h"
#include "resources.h"

#include "render_object_mesh.h"

namespace render {

object_cook::object_cook( lib::reader const& in_config ) :
	m_config( in_config )
{ }

void object_cook::create_resource( )
{
	u32 const max_queries = 4096;

	lib::buffer_array<task_info> queries;
	queries.create( stack_allocate( max_queries * sizeof(task_info) ), max_queries );

	u16 const mesh_count = m_config.read<u16>( );

	ASSERT( mesh_count != 0 );

	m_result = g_resources.render_allocator( ).allocate( object::calculate_allocation_size( mesh_count ) );
	m_result->m_mesh_count = mesh_count;

	u16 i = 0;

	m_result->for_each_mesh_object( [this, &queries]( render_object_mesh* const in_obj )
	{
		new ( in_obj ) render_object_mesh( );
		in_obj->create( m_config, queries );
	} );

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

void object_cook::on_child_resources_ready( queried_resources& in_queried )
{
	u32 const max_task_count = 4096;
	lib::buffer_array<gpu_upload_task> tasks;
	tasks.create( stack_allocate( sizeof(gpu_upload_task) * max_task_count ), max_task_count );

	uptr const allocator_size = 64 * Kb;
	linear_allocator allocator;
	allocator.create( stack_allocate( allocator_size ), allocator_size );

	m_result->for_each_mesh_object( [this, &in_queried, &tasks, &allocator]( render_object_mesh* const in_obj )
	{
		in_obj->on_resources_ready( in_queried, tasks, allocator, false );
	} );

	ASSERT( tasks.size( ) != 0 );

	custom_task_context context = create_child_custom_tasks(
		(u32)tasks.size( ),
		resource_system::user_callback_task<object_cook, &object_cook::on_copy_tasks_finished>( this )
	);

	lib::for_each( tasks.begin( ), tasks.end( ), [context]( gpu_upload_task& in_task )
	{
		in_task.set_task_context( context );
	} );

	g_gpu_uploader.push_background_tasks( tasks.data( ), (u32)tasks.size( ) );
}

void object_cook::on_copy_tasks_finished( )
{
	finish( m_result );
}

} // namespace render