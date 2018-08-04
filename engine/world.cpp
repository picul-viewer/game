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

		default:
			return DefWindowProcA( hwnd, message, w_param, l_param );
	}

	return 0;
}

static void window_thread( pvoid )
{
	g_world.window_thread( );
}

};

void world::window_resize( math::u32x2 const& new_dimensions )
{
	if ( new_dimensions == m_window_dimensions )
		return;

	m_window_dimensions = new_dimensions;

	// Resize
}

void world::window_activate( bool is_active )
{
	if ( is_active == m_is_active )
		return;

	m_is_active = is_active;

	// Activate/deactivate
}

void world::window_input( pvoid handle )
{
	// Input
}

void world::window_thread( )
{
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );
	m_window_created = true;
	m_window.run( );
	m_window.destroy( );
}

void world::initialize( )
{
	m_window_created = false;
	m_alive = true;

	m_window_thread.create( &helper::window_thread, 1 * Mb, nullptr );
}

void world::deinitialize( )
{
	m_window_thread.destroy( INFINITE );
}

void world::run( math::u32x2 in_window_dimensions )
{
	m_window_dimensions = in_window_dimensions;

	initialize( );

	while ( !m_window_created )
		_mm_pause( );

	ASSERT( ( ( m_window_dimensions.x & 0xFFFF ) == m_window_dimensions.x ) &&
			( ( m_window_dimensions.y & 0xFFFF ) == m_window_dimensions.y ) );

	render::g_world.run( (HWND)m_window.get_hwnd( ), m_window_dimensions, true, true );

	deinitialize( );
}

void world::exit( )
{
	m_alive = false;
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

void world::set_current_camera( camera* in_camera ) const
{
	render::g_world.set_current_camera( in_camera );
}

world g_world;

} // namespace engine