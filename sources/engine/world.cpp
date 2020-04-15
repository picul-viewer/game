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

#include <render/api.h>

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

void world::main_thread( )
{
	u32 const thread_index = engine_thread_main;
	utils::set_thread_index( thread_index );

	// Waiting for window to create.
	m_window_event.wait( );

	ASSERT( ( m_window_dimensions.x < 0x10000 ) && ( m_window_dimensions.y < 0x10000 ) );
	render::create( (HWND)m_window.get_hwnd( ), m_window_dimensions, true );

	// Wait for render to create.
	while ( !render::ready( ) )
		while ( resource_system::process_task( thread_index ) );

	game::world_interface::create( );

	// Waiting for game to create.
	while ( !game::world_interface::ready( ) )
		while ( resource_system::process_task( thread_index ) );

	while ( m_alive )
	{
		game::world_interface::update( );
		render::update( );
	}

	// Exit from main.
	m_main_event.set( );
	m_window_event.wait( );

	game::world_interface::destroy( );

	// Waiting for game to destroy.
	while ( !game::world_interface::ready( ) )
		while ( resource_system::process_task( thread_index ) );

	render::destroy( );

	// Waiting for render to destroy.
	while ( !render::ready( ) )
		while ( resource_system::process_task( thread_index ) );
}

void world::window_thread( )
{
	u32 const thread_index = engine_thread_window;
	utils::set_thread_index( thread_index );

	sys::g_window_input.create( );
	m_window.create( "game", m_window_dimensions, false, &helper::window_procedure );

	// Wake main thread.
	m_window_event.set( );

	m_window.run( );
	exit( );

	// Exit from window.
	m_main_event.wait( );
	m_window_event.set( );

	m_window.destroy( );
	sys::g_window_input.destroy( );
}

void world::fs_thread( )
{
	u32 const thread_index = engine_thread_fs;
	utils::set_thread_index( thread_index );

	sys::system_event const& queue_event = resource_system::queue_event( thread_index );

	while ( m_resource_system_alive )
	{
		bool const empty = !resource_system::process_task( thread_index );

		if ( empty )
			queue_event.wait( );
	}
}

void world::render_copy_thread( )
{
	u32 const thread_index = engine_thread_render_copy;
	utils::set_thread_index( thread_index );

	render::copy_thread_job( thread_index, m_resource_system_alive );
}

void world::helper_thread( )
{
	u32 const thread_index = engine_helper_threads_first + interlocked_exchange_add( m_helper_count, 1 );
	utils::set_thread_index( thread_index );

	sys::system_event events_to_wait[] = { resource_system::queue_event( thread_index ), resource_system::helper_queue_event( ) };

label_outer_loop:
	while ( m_resource_system_alive )
	{
		while ( resource_system::process_task( thread_index ) );

		while ( m_resource_system_alive )
		{
			bool const empty = !resource_system::process_helper_task( );

			if ( empty )
			{
				u32 const index = sys::system_event::wait_any( (u32)array_size( events_to_wait ), events_to_wait );

				ASSERT( index < (u32)array_size( events_to_wait ) );

				if ( index == 0 )
					goto label_outer_loop;
			}
		}
	}
}

void world::create( )
{
	// Engine is running.
	m_alive = true;

	m_resource_system_alive = true;
	g_resources.create( );

	m_main_event.create( false, false );
	m_window_event.create( false, false );

	u32 const cpu_cores = sys::thread::hardware_concurrency( );
	u32 const helper_threads_count = math::clamp( cpu_cores - engine_base_threads_count, 1u, (u32)engine_helper_threads_count );
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

	m_threads[engine_thread_render_copy].create( sys::thread::method_helper<world, &world::render_copy_thread>, 4 * Mb, this );
	m_threads[engine_thread_render_copy].set_priority( 13 );
	m_threads[engine_thread_render_copy].force_processor_index( processor_index++ % cpu_cores );

	for ( u32 i = 0; i < helper_threads_count; ++i )
	{
		m_threads[engine_helper_threads_first + i].create( sys::thread::method_helper<world, &world::helper_thread>, 4 * Mb, this );
		m_threads[engine_helper_threads_first + i].set_priority( 13 );
		m_threads[engine_helper_threads_first + i].force_processor_index( processor_index++ % cpu_cores );
	}
}

void world::destroy( )
{
	m_resource_system_alive = false;
	resource_system::stop( );

	sys::thread::destroy( m_thread_count - 1, m_threads + 1 );

	m_main_event.destroy( );
	m_window_event.destroy( );
	
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
	render::set_current_scene( &in_scene->render_scene( ) );
}

world g_world;

} // namespace engine