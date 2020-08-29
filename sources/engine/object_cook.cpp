#include "object_cook.h"
#include <lib/allocator.h>
#include "scene.h"
#include "resources.h"

#include <render/object_cook.h>

namespace engine {

object_cook::object_cook( lib::reader const& in_config ) :
	m_config( in_config )
{ }

void object_cook::create_resource( )
{
	m_result = g_resources.get_object_allocator( ).allocate( );

	u32 const render_size = m_config.read<u32>( );
	pvoid const render_data = m_config.read_data( render_size );

	render::object_cook* const render_cook = create_cook<render::object_cook>(
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