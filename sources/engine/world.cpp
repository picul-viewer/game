#include "world.h"
#include <Windows.h>

#include <macros.h>
#include <math/math_common.h>

#include <resource_system/api.h>

#include <system/interlocked.h>
#include <system/time.h>
#include <system/window.h>
#include <system/window_input.h>

#include "resources.h"

#include <render/world.h>

#include <ui/font.h>

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

void world::set_game_ready( )
{
	// Everytime it's just about waking the main thread.
	resource_system::break_thread( engine_thread_main );
}

void world::main_thread( )
{
	utils::set_thread_index( engine_thread_main );

	// TODO: do this appropriately.
	ui::font::container( ).create( nullptr );

	// Waiting for window to create.
	resource_system::process_free( );

	ASSERT( ( m_window_dimensions.x < 0x10000 ) && ( m_window_dimensions.y < 0x10000 ) );
	render::g_world.create( (HWND)m_window.get_hwnd( ), m_window_dimensions, true );

	// NOTE: render create/destroy are simultaneous for now, so do not wait.
	// Change it when needed.

	game::world_interface::create( );

	// Waiting for game to create.
	resource_system::process_free( );

	sys::time const max_frame_time = 1.0 / 60.0;
	sys::time last_tick = sys::time::now( );

	while ( m_alive )
	{
		game::world_interface::update( );
		render::g_world.update( );

		resource_system::process_busy( last_tick + max_frame_time );
		last_tick = sys::time::now( );
	}

	// Exit from main.
	m_exit_events[engine_thread_main].set( );

	// Waiting for all engine systems to stop.
	sys::system_event::wait_all( engine_busy_threads_count, m_exit_events );

	game::world_interface::destroy( );

	// Waiting for game to destroy.
	resource_system::process_free( );

	render::g_world.destroy( );

	// NOTE: render create/destroy are simultaneous for now, so do not wait.
	// Change it when needed.

	ui::font::container( ).destroy( );
}

void world::window_thread( )
{
	utils::set_thread_index( engine_thread_window );

	sys::g_window_input.create( );
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );

	// Wake main thread.
	resource_system::break_thread( engine_thread_main );

	m_window.run( );
	exit( );

	// Exit from window.
	m_exit_events[engine_thread_window].set( );
	
	// Waiting for all engine systems to stop.
	sys::system_event::wait_all( engine_busy_threads_count, m_exit_events );

	// Waiting for render to destroy.
	resource_system::process_free( );

	m_window.destroy( );
	sys::g_window_input.destroy( );
}

void world::fs_thread( )
{
	utils::set_thread_index( engine_thread_fs );

	resource_system::process_free( );
}

void world::helper_thread( )
{
	u32 const thread_index = engine_helper_threads_first + interlocked_exchange_add( m_helper_count, 1 );
	utils::set_thread_index( thread_index );

	resource_system::process_helper( );
}

void world::create( )
{
	// Engine is running.
	m_alive = true;

	g_resources.create( );

	for ( u32 i = 0; i < engine_busy_threads_count; ++i )
		m_exit_events[i].create( false, true );

	u32 const cpu_cores = sys::thread::hardware_concurrency( );
	u32 const helper_threads_count = math::clamp( cpu_cores - engine_helper_threads_first, 1u, (u32)engine_helper_threads_count );
	m_thread_count = engine_helper_threads_first + helper_threads_count;

	resource_system::create( m_thread_count );

	u32 processor_index = 0;

	m_threads[engine_thread_main] = sys::thread::get_current( );
	m_threads[engine_thread_main].set_priority( 17 );
	m_threads[engine_thread_main].force_processor_index( processor_index++ % cpu_cores );

	m_threads[engine_thread_window].create( sys::thread::method_helper<world, &world::window_thread>, 4 * Mb, this );
	m_threads[engine_thread_window].set_priority( 17 );
	m_threads[engine_thread_window].force_processor_index( processor_index++ % cpu_cores );

	m_threads[engine_thread_fs].create( sys::thread::method_helper<world, &world::fs_thread>, 4 * Mb, this );
	m_threads[engine_thread_fs].set_priority( 17 );
	m_threads[engine_thread_fs].force_processor_index( processor_index++ % cpu_cores );

	for ( u32 i = 0; i < helper_threads_count; ++i )
	{
		m_threads[engine_helper_threads_first + i].create( sys::thread::method_helper<world, &world::helper_thread>, 4 * Mb, this );
		m_threads[engine_helper_threads_first + i].set_priority( 13 );
		m_threads[engine_helper_threads_first + i].force_processor_index( processor_index++ % cpu_cores );
	}
}

void world::destroy( )
{
	resource_system::stop( );

	sys::thread::destroy( m_thread_count - 1, m_threads + 1 );

	for ( u32 i = 0; i < engine_busy_threads_count; ++i )
		m_exit_events[i].destroy( );
	
	resource_system::destroy( );

	g_resources.destroy( );
}

void world::run( math::u32x2 const in_window_dimensions )
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

void world::set_current_scene( scene* const in_scene ) const
{
	render::g_world.set_current_scene( &in_scene->render_scene( ) );
}

world g_world;

} // namespace engine