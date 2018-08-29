#include "world.h"
#include <macros.h>
#include "render_api.h"
#include "resources.h"
#include "render_parameters_manager.h"
#include "renderer.h"

namespace render {

world::world( ) :
	m_alive		( false ),
	m_created	( false )
{ }

void world::run( pvoid in_hwnd, math::u16x2 in_resolution, bool in_is_windowed, bool in_allow_debug )
{
	m_alive						= true;

	parameters& params			= get_parameters( );
	params.hwnd					= in_hwnd;
	params.screen_resolution	= in_resolution;
	params.is_windowed			= in_is_windowed;
	params.is_d3d_debug			= in_allow_debug;
	g_parameters_manager.update	( );

	g_renderer.create			( );
	m_created					= true;

	do
		g_renderer.render		( );
	while ( m_alive );

	g_renderer.destroy			( );
}

void world::exit( )
{
	m_alive						= false;
}

bool world::is_created( ) const
{
	return m_created;
}

parameters& world::get_parameters( ) const
{
	return g_parameters_manager.get_parameters_for_modification( );
}

scene* world::create_scene( ) const
{
	return g_resources.get_scene_pool( ).allocate( sizeof(scene) );
}

void world::set_current_scene( scene* in_scene ) const
{
	g_renderer.set_scene( in_scene );
}

void world::destroy_scene( scene* in_scene ) const
{
	g_resources.get_scene_pool( ).deallocate( in_scene );
}

object* world::create_object( ) const
{
	return g_resources.get_object_pool( ).allocate( sizeof(object) );
}

void world::destroy_object( object* in_object ) const
{
	g_resources.get_object_pool( ).deallocate( in_object );
}

world g_world;

} // namespace render