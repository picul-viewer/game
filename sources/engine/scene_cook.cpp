#include "scene_cook.h"
#include <lib/allocator.h>
#include "resources.h"

#include <render/scene_cook.h>

namespace engine {

scene_cook* scene_cook::create( lib::reader const& in_config )
{
	scene_cook* const result = std_allocator( ).allocate( sizeof(scene_cook) );

	result->init( );

	result->m_config = in_config;

	return result;
}

void scene_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void scene_cook::create_resource( )
{
	m_result = g_resources.get_scene_allocator( ).allocate( );

	u32 const render_size = m_config.read<u32>( );
	pvoid const render_data = m_config.read_data( render_size );

	render::scene_cook* const render_cook = render::scene_cook::create(
		&m_result->m_render,
		lib::reader( render_data, render_size )
	);

	create_child_resources( callback_task<&scene_cook::on_subsystems_ready>( ), render_cook );
}

void scene_cook::on_subsystems_ready( queried_resources& in_queried )
{
	render::scene::ptr render = in_queried.get_resource<render::scene::ptr>( );
	ASSERT( render != nullptr );
	render.reset( );

	finish( m_result );
}

} // namespace engine