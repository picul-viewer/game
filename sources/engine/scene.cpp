#include "scene.h"

#include <render/world.h>
#include <render/scene.h>

#include "object.h"

namespace engine {

void scene::create( bool in_create_render )
{
	if ( in_create_render )
	{
		m_render = render::g_world.create_scene( );
		m_render->create( );
	}
}

void scene::destroy( )
{
	if ( m_render )
	{
		m_render->destroy( );
		render::g_world.destroy_scene( m_render );
	}
}

void scene::remove_all_static_objects( )
{
	if ( m_render )
		m_render->remove_all_static_objects( );
}

void scene::insert_static_object( object* in_object )
{
	if ( m_render )
		m_render->insert_static_object( in_object->get_render( ) );
}

void scene::build_static_scene( )
{
	if ( m_render )
		m_render->build_static_scene( );
}

void scene::make_current( ) const
{
	if ( m_render )
		render::g_world.set_current_scene( m_render );
}

} // namespace engine