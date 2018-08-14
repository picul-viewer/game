#include "object.h"

#include <render/object.h>
#include <render/world.h>

namespace engine {

void object::create( binary_config& in_config )
{
	bool const create_render = in_config.read<bool>( );
		
	if ( create_render )
	{
		m_render = render::g_world.create_object( );
		m_render->create( in_config );
	}
}

void object::destroy( )
{
	if ( m_render )
	{
		m_render->destroy( );
		render::g_world.destroy_object( m_render );
	}
}

} // namespace engine