#include "world.h"
#include <macros.h>

#include <Windows.h>

#include <system/window.h>
#include <system/window_input.h>
#include <system/interlocked.h>

#include "resources.h"

#include <render/world.h>

namespace engine {

struct world::helper
{

static LRESULT CALLBACK window_procedure( HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param )
{
	switch ( message )
	{
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			break;
		}
		case WM_SIZE:
		{
			math::u32x2 const& dimensions = math::u32x2( (u32)LOWORD( l_param ), (u32)HIWORD( l_param ) );
			g_world.window_resize( dimensions );

			break;
		}
		case WM_ACTIVATE:
		{
			bool const is_active = ( w_param != 0 );
			g_world.window_activate( is_active );

			break;
		}
		case WM_INPUT:
		{
			g_world.window_input( (pvoid)l_param );
			break;
		}
	}

	return DefWindowProc( hwnd, message, w_param, l_param );
}

};

void world::window_resize( math::u32x2 const& new_dimensions )
{
	if ( new_dimensions == m_window_dimensions )
		return;

	m_window_dimensions = new_dimensions;

	game::world_interface::window_resize( new_dimensions );
}

void world::window_activate( bool is_active )
{
	if ( is_active == m_is_active )
		return;

	m_is_active = is_active;

	game::world_interface::window_activate( is_active );
}

void world::window_input( pvoid handle )
{
	game::world_interface::window_input( handle );
}

void world::window_thread( )
{
	sys::g_window_input.create( );
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );
	change_system_alive( system_window );

	// Waiting for game logic to create.
	SPIN_LOCK( ( m_systems_alive & system_game_mask ) == 0 );

	m_window.run( );
	change_system_running( system_window );

	// This needed if window was closed.
	exit( );

	// Waiting for game logic to destroy.
	SPIN_LOCK( ( m_systems_alive & system_game_mask ) == system_game_mask );

	m_window.destroy( );
	sys::g_window_input.destroy( );
	change_system_alive( system_window );
}

void world::render_thread( )
{
	// Waiting for window to create.
	SPIN_LOCK( ( m_systems_alive & system_window_mask ) == 0 );

	ASSERT( ( m_window_dimensions.x < 0x10000 ) && ( m_window_dimensions.y < 0x10000 ) );

	render::g_world.create( (HWND)m_window.get_hwnd( ), m_window_dimensions, true, true );
	change_system_alive( system_render );

	// Waiting for game logic to create.
	SPIN_LOCK( ( m_systems_alive & system_game_mask ) == 0 );

	while ( m_alive )
		render::g_world.update( );
	change_system_running( system_render );

	// Waiting for game logic to destroy.
	SPIN_LOCK( ( m_systems_alive & system_game_mask ) == system_game_mask );

	render::g_world.destroy( );
	change_system_alive( system_render );
}

void world::game_thread( )
{
	// Waiting for all engine systems to create.
	SPIN_LOCK( ( m_systems_alive & engine_systems_mask ) != engine_systems_mask );

	game::world_interface::create( );
	change_system_alive( system_game );

	while ( m_alive )
		game::world_interface::update( );
	change_system_running( system_game );

	// Waiting for all engine systems to stop.
	SPIN_LOCK( ( m_systems_running & engine_systems_mask ) != 0 );

	game::world_interface::destroy( );
	change_system_alive( system_game );
}

void world::create( )
{
	// Engine is running.
	m_alive = true;
	// All systems are not alive yet.
	m_systems_alive = 0;
	// All systems are in running state.
	m_systems_running = ( 1 << system_count ) - 1;

	g_resources.create( );

	m_threads[thread_window].create( thread::method_helper<world, &world::window_thread>, 1 * Mb, this );
	m_threads[thread_game].create( thread::method_helper<world, &world::game_thread>, 4 * Mb, this );
}

void world::destroy( )
{
	m_threads->destroy( INFINITE, thread_count );
	
	g_resources.destroy( );
}

void world::run( math::u32x2 in_window_dimensions )
{
	m_window_dimensions = in_window_dimensions;

	create( );

	// Do render in the main thread.
	render_thread( );

	destroy( );
}

void world::exit( )
{
	m_alive = false;

	m_window.exit( );
}

scene* world::create_scene( ) const
{
	scene* new_scene = g_resources.get_scene_pool( ).allocate( sizeof(scene) );
	new_scene->create( true );
	return new_scene;
}

void world::set_current_scene( scene* in_scene ) const
{
	in_scene->make_current( );
}

void world::destroy_scene( scene* in_scene ) const
{
	in_scene->destroy( );
	g_resources.get_scene_pool( ).deallocate( in_scene );
}

void world::change_system_alive( system s )
{
	interlocked_xor( m_systems_alive, 1 << s );
}

void world::change_system_running( system s )
{
	interlocked_xor( m_systems_running, 1 << s );
}

world g_world;

} // namespace engine