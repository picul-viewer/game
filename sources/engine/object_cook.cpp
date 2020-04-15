#include "object_cook.h"
#include <lib/allocator.h>
#include "scene.h"
#include "resources.h"

#include <render/object_cook.h>

namespace engine {

object_cook* object_cook::create( lib::reader const& in_config )
{
	object_cook* const result = std_allocator( ).allocate( sizeof(object_cook) );

	result->init( );

	result->m_config = in_config;

	return result;
}

void object_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void object_cook::create_resource( )
{
	m_result = g_resources.get_object_allocator( ).allocate( );

	u32 const render_size = m_config.read<u32>( );
	pvoid const render_data = m_config.read_data( render_size );

	render::object_cook* const render_cook = render::object_cook::create(
		lib::reader( render_data, render_size )
	);

	create_child_resources( callback_task<&object_cook::on_subsystems_ready>( ), render_cook );
}

void object_cook::on_subsystems_ready( queried_resources& in_queried )
{
	m_result->m_render = in_queried.get_resource<render::object::ptr>( );
	ASSERT( m_result->m_render != nullptr );

	finish( m_result );
}

} // namespace engine