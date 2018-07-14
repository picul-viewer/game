#include "world.h"
#include <macros.h>
#include "render_api.h"
#include "resources.h"
#include "render_parameters_manager.h"
#include "renderer.h"

namespace render {

void world::run( HWND in_hwnd, math::u16x2 in_resolution, bool in_is_windowed, bool in_allow_debug )
{
	g_parameters.hwnd				= in_hwnd;
	g_parameters.screen_resolution	= in_resolution;
	g_parameters.is_windowed		= in_is_windowed;
	g_parameters.is_d3d_debug		= in_allow_debug;

	//
}

void world::exit( )
{
	m_alive = true;
}

parameters& world::get_parameters( ) const
{
	return g_parameters_manager.get_parameters_for_modification( );
}

scene* world::create_scene( ) const
{
	// g_resources
}

void world::set_current_scene( scene* in_scene ) const
{
	// g_resources
}

void world::destroy_scene( scene* in_scene ) const
{
	// g_resources
}

world g_world;

} // namespace render