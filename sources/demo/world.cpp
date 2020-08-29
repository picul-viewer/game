#include "world.h"
#include <engine/scene_cook.h>
#include <engine/object_cook.h>
#include <ui/font_cook.h>
#include <engine/world.h>

#include <render/api.h>

#include <lib/allocator.h>
#include <lib/dynamic_writer.h>

#include <math/math_3d.h>

#include <system/input.h>
#include <system/window_input.h>

#include <resources/resources_path.h>

#include <macros.h>
#include <Windows.h>

#include <resource_system/api.h>
#include <resources/raw_data.h>

#include <system/file.h>

namespace game {

char scene_cook_data[256];
char object_cook_data[1 * Mb];

void world::create( )
{
	m_ready = false;

	raw_data_cook* const cook = create_cook<raw_data_cook>( GET_RESOURCE_PATH( "configs/render/scenes/sponza_lit.scene.cfg" ) );

	resource_system::create_resources(
		resource_system::user_callback_task<world, &world::on_scene_loaded>( this ),
		cook
	);
}

void world::on_scene_loaded( queried_resources& in_queried )
{
	m_scene_data = in_queried.get_resource<raw_data::ptr>( );

	ui::font_cook* const font_cook = create_cook<ui::font_cook>( GET_RESOURCE_PATH( "configs/fonts/console.font.cfg" ) );

	engine::scene_cook* const scene_cook = create_cook<engine::scene_cook>( lib::reader( m_scene_data->data( ), m_scene_data->size( ) ) );

	resource_system::create_resources(
		resource_system::user_callback_task<world, &world::on_resources_ready>( this ),
		font_cook, scene_cook
	);
}

void world::on_resources_ready( queried_resources& in_queried )
{
	m_scene_data->destroy( );
	m_scene_data.reset( );

	m_console_font = in_queried.get_resource<ui::font::ptr>( );

	m_scene = in_queried.get_resource<engine::scene::ptr>( );

	render::get_parameters( ).camera.fov = math::degree_to_radian( 60.0f );

	engine::g_world.set_current_scene( m_scene );

	m_camera.create( math::float3( 0.0f ), math::float2( 0.0f ), math::float3( 2.0f, 5.0f, 15.0f ), 0.002f );

	initialize_console( );

	m_ready = true;
}

void world::initialize_console( )
{
	m_console_visible = false;

	m_console.create( m_console_font );

	m_console.set_default_colors( );
	
	m_console.set_border_size( 5 );
	m_console.set_corners_position( math::u16x4( 0, 0, engine::g_world.get_window_dimensions( ).x, engine::g_world.get_window_dimensions( ).y / 4 ) );

	m_console.set_callback( &on_console_command );
}

void world::on_console_command( pcstr const str )
{
	if ( strings::equal( str, "quit" ) )
		engine::g_world.exit( );
}

void world::update( )
{
	float const elapsed_time = (float)m_ticker.tick( );
	m_camera.update( elapsed_time );

	if ( !m_console_visible )
		render::get_parameters( ).camera.view = m_camera.get_view_matrix( );
	else
		m_console.render( );
}

void world::destroy( )
{
	m_ready = false;

	m_console.destroy( );

	m_console_font->release( );
	m_console_font.reset( );

	resource_system::destroy_resources(
		resource_system::user_callback_task<world, &world::on_resources_destroyed>( this ),
		m_scene.get( )
	);

	m_scene.reset( );
}

void world::on_resources_destroyed( )
{
	m_ready = true;
}

bool world::ready( )
{
	return m_ready;
}

void world::window_resize( math::u32x2 const& new_dimensions )
{
}

void world::window_activate( bool const is_active )
{
}

void world::window_char( u32 const key )
{
	if ( key == 0x60 ) // tilde
	{
		m_console_visible = !m_console_visible;
		m_camera.enable( !m_console_visible );
		return;
	}

	if ( m_console_visible )
		m_console.on_char( key );
}

void world::window_input( )
{
	using namespace sys;

	RAWINPUT& data			= *(RAWINPUT*)g_window_input.get_raw_input( );

	if (data.header.dwType == RIM_TYPEKEYBOARD)
	{
		key const k			= (key)data.data.keyboard.VKey;
		bool const pressed	= !( data.data.keyboard.Flags & 0x1 );

		if ( ( k == key::escape ) && pressed )
		{
			if ( m_console_visible )
			{
				m_console_visible = false;
				m_camera.enable( true );
			}
			else
				engine::g_world.exit( );
		}
		else if ( ( k == key::f5 ) && pressed )
		{
			render::reload( );
		}
		else if ( ( k == key::f9 ) && pressed )
		{
			render::get_parameters( ).draw_statistics = !render::get_parameters( ).draw_statistics;
		}
		else if ( m_console_visible )
			m_console.on_input( );
	}
}

world g_world;

} // namespace game