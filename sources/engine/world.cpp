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
		case WM_CHAR:
		{
			u32 const key = (u32)w_param;
			g_world.window_char( key );

			break;
		}
		case WM_INPUT:
		{
			sys::g_window_input.on_message( (pvoid)l_param );
			g_world.window_input( );
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

void world::window_activate( bool const is_active )
{
	if ( is_active == m_is_active )
		return;

	m_is_active = is_active;

	game::world_interface::window_activate( is_active );
}

void world::window_char( u32 const key )
{
	game::world_interface::window_char( key );
}

void world::window_input( )
{
	game::world_interface::window_input( );
}

void world::window_thread( )
{
	sys::g_window_input.create( );
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );
	m_alive_events[event_window].set( );

	// Waiting for all systems to create.
	sys::system_event::wait_all( event_count, m_alive_events );

	m_window.run( );
	exit( );

	// Reset owned alive event.
	m_alive_events[event_window].reset( );
	
	// Exit from window.
	m_exit_events[event_window].set( );
	
	// Waiting for all engine systems to stop.
	sys::system_event::wait_all( event_count, m_exit_events );

	// Waiting for render to destroy.
	m_alive_events[event_main].wait( );

	m_window.destroy( );
	sys::g_window_input.destroy( );
	m_alive_events[event_window].set( );
}

void world::main_thread( )
{
	// Waiting for window to create.
	sys::system_event::wait_all( system_event_count, m_alive_events );

	ASSERT( ( m_window_dimensions.x < 0x10000 ) && ( m_window_dimensions.y < 0x10000 ) );
	render::g_world.create( (HWND)m_window.get_hwnd( ), m_window_dimensions, true );

	game::world_interface::create( );

	m_alive_events[event_main].set( );

	while ( m_alive )
	{
		game::world_interface::update( );
		render::g_world.update( );
	}
	
	// Reset owned alive event.
	m_alive_events[event_main].reset( );
	
	// Exit from main.
	m_exit_events[event_main].set( );

	// Waiting for window to stop.
	sys::system_event::wait_all( event_count, m_exit_events );

	game::world_interface::destroy( );
	render::g_world.destroy( );

	m_alive_events[event_main].set( );
}

void world::create( )
{
	static_assert( event_main == event_count - 1, "revisit synchronization code" );

	// Engine is running.
	m_alive = true;

	g_resources.create( );

	for ( u32 i = 0; i < event_count; ++i )
	{
		m_alive_events[i].create( false, true );
		m_exit_events[i].create( false, true );
	}

	m_threads[thread_window].create( sys::thread::method_helper<world, &world::window_thread>, 1 * Mb, this );
}

void world::destroy( )
{
	sys::thread::destroy( thread_count, m_threads );

	for ( u32 i = 0; i < event_count; ++i )
	{
		m_alive_events[i].destroy( );
		m_exit_events[i].destroy( );
	}
	
	g_resources.destroy( );
}

void world::run( math::u32x2 in_window_dimensions )
{
	m_window_dimensions = in_window_dimensions;

	create( );

	main_thread( );

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

world g_world;

} // namespace engine