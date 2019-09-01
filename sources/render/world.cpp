#include "world.h"
#include <macros.h>
#include "api.h"
#include "resources.h"
#include "parameters_manager.h"
#include "renderer.h"

namespace render {

void world::create( pvoid in_hwnd, math::u16x2 in_resolution, bool in_is_windowed )
{
	parameters& params			= get_parameters( );
	params.hwnd					= in_hwnd;
	params.screen_resolution	= in_resolution;
	params.is_windowed			= in_is_windowed;
	params.draw_statistics		= false;

	g_parameters_manager.update	( );

	g_renderer.create			( );
}

void world::update( )
{
	g_renderer.render			( );
}

void world::destroy( )
{
	g_renderer.destroy			( );
}

parameters& world::get_parameters( ) const
{
	return g_parameters_manager.get_parameters_for_modification( );
}

void world::set_current_scene( scene* in_scene ) const
{
	g_renderer.set_scene( in_scene );
}

ui_batch& world::get_ui_batch( ) const
{
	return g_resources.get_ui_batch( );
}

world g_world;

} // namespace render