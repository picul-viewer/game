#include <engine/world.h>
#include "world.h"

void game::world_interface::run( )
{
	game::g_world.run( );
}

void game::world_interface::exit( )
{
	game::g_world.exit( );
}

void game::world_interface::window_resize( math::u32x2 const& new_dimensions )
{
	game::g_world.window_resize( new_dimensions );
}

void game::world_interface::window_activate( bool is_active )
{
	game::g_world.window_activate( is_active );
}

void game::world_interface::window_input( pvoid handle )
{
	game::g_world.window_input( handle );
}