#include "world.h"
#include <macros.h>

#include <Windows.h>

#include <system/window.h>
#include <system/window_input.h>

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

world::world( ) :
	m_window_created( false )
{ }

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

void world::render_thread( )
{
	SPIN_LOCK( !m_window_created );

	ASSERT( ( ( m_window_dimensions.x & 0xFFFF ) == m_window_dimensions.x ) &&
			( ( m_window_dimensions.y & 0xFFFF ) == m_window_dimensions.y ) );

	render::g_world.run( (HWND)m_window.get_hwnd( ), m_window_dimensions, true, true );
	
	LOG( "engine: render exit\n" );

	m_window.exit( );
}

void world::window_thread( )
{
	sys::g_window_input.create( );
	
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );
	m_window_created = true;

	m_window.run( );
	m_window.destroy( );
	
	sys::g_window_input.destroy( );

	LOG( "engine: window exit\n" );
	
	game::world_interface::exit( );
}

void world::game_thread( )
{
	game::world_interface::run( );
	
	LOG( "engine: game logic exit\n" );

	render::g_world.exit( );
}

void world::initialize( )
{
	g_resources.create( );

	m_threads[thread_window].create( thread::method_helper<world, &world::window_thread>, 1 * Mb, this );
	
	m_threads[thread_game].create( thread::method_helper<world, &world::game_thread>, 4 * Mb, this );
}

void world::deinitialize( )
{
	m_threads->destroy( INFINITE, thread_count );
	
	g_resources.destroy( );
}

void world::run( math::u32x2 in_window_dimensions )
{
	m_window_dimensions = in_window_dimensions;

	initialize( );

	// Do render in the main thread.
	render_thread( );

	deinitialize( );
}

void world::exit( )
{
	/*
		Current destruction sequence:
		1. Window
		2. Game logic thread
		3. Render thread
	*/
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