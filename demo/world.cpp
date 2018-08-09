#include "world.h"
#include <engine/world.h>

#include <system/input.h>
#include <system/window_input.h>

#include <macros.h>
#include <immintrin.h>
#include <Windows.h>

namespace game {

void world::run( )
{
	m_alive = true;

	while ( m_alive )
		_mm_pause( );
}

void world::exit( )
{
	m_alive = false;
}

void world::window_resize( math::u32x2 const& new_dimensions )
{
	LOG( "window resized: ( %d, %d )\n", new_dimensions.x, new_dimensions.y );
}

void world::window_activate( bool is_active )
{
	LOG( "window %s" "activated\n", is_active ? "" : "de" );
}

void world::window_input( pvoid handle )
{
	using namespace sys;

	RAWINPUT data;
	g_window_input.get_input_data( handle, &data );

	if (data.header.dwType == RIM_TYPEKEYBOARD)
    {
		key const k			= (key)data.data.keyboard.VKey;
		bool const pressed	= !( data.data.keyboard.Flags & 0x1 );

		if ( ( k == key::escape ) && pressed )
		{
			LOG( "Exit\n" );
			engine::g_world.exit( );
		}
    }
}

world g_world;

} // namespace game