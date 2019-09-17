#include "world.h"
#include <engine/scene_cook.h>
#include <engine/object_cook.h>
#include <ui/font_cook.h>
#include <engine/world.h>

#include <render/world.h>

#include <lib/allocator.h>
#include <lib/dynamic_writer.h>

#include <math/math_3d.h>

#include <system/input.h>
#include <system/window_input.h>

#include <resources/resources_path.h>

#include <render/ui_batch.h>

#include <macros.h>
#include <Windows.h>

#include <resource_system/api.h>
#include <resources/raw_data.h>

namespace game {

char scene_cook_data[256];
char object_cook_data[1 * Mb];

void world::create( )
{
	uptr scene_cook_size;
	uptr object_cook_size;

	{
		// Empty scene.
		lib::writer w( scene_cook_data, 256 );

		u32* const render_size = w.ptr( );
		w += sizeof(u32);
		pointer const render_data = w.ptr( );

		w.write<u32>( 0 );
		w.write<u32>( 0 );

		*render_size = (u32)w.size( render_data );

		scene_cook_size = w.size( scene_cook_data );
	}

	{
		u32 const grid_size = 10;

		// Box object.
		lib::writer w( object_cook_data, 1 * Mb );

		u32* const render_size = w.ptr( );
		w += sizeof(u32);
		pointer const render_data = w.ptr( );

		w.write<u16>( grid_size * grid_size * grid_size );

		for ( u32 i = 0; i < grid_size; ++i )
		for ( u32 j = 0; j < grid_size; ++j )
		for ( u32 k = 0; k < grid_size; ++k )
		{
			float const dimension = grid_size * 2.0f + ( grid_size - 1 ) * 0.5f;

			w.write<u8>( 0 );
			w.write_str( "configs/box.render_model_mesh" );

			w.write<math::float4x3>(
				math::float4x3(
					math::float4( 1.0f, 0.0f, 0.0f, i * 2.5f + 1.0f - dimension * 0.5f ),
					math::float4( 0.0f, 1.0f, 0.0f, j * 2.5f + 1.0f - dimension * 0.5f ),
					math::float4( 0.0f, 0.0f, 1.0f, k * 2.5f + 1.0f - dimension * 0.5f )
				)
			);
		}

		*render_size = (u32)w.size( render_data );

		object_cook_size = w.size( object_cook_data );
	}

	task_info queries[64];
	u32 query_index = 0;

	ui::font_cook* const font_cook = ui::font_cook::create( GET_RESOURCE_PATH( "configs\\fonts\\console.cfg" ) );
	font_cook->fill_task_info( queries[query_index++] );

	engine::scene_cook* const scene_cook = engine::scene_cook::create( lib::reader( scene_cook_data, scene_cook_size ) );
	scene_cook->fill_task_info( queries[query_index++] );

	engine::object_cook* const object_cook = engine::object_cook::create( lib::reader( object_cook_data, object_cook_size ) );
	object_cook->fill_task_info( queries[query_index++] );

	resource_system::create_resources(
		queries, query_index,
		resource_system::user_callback_task<world, &world::on_resources_ready>( this )
	);
}

void world::on_resources_ready( queried_resources& resources )
{
	m_console_font = resources.get_resource<ui::font::ptr>( );

	m_scene = resources.get_resource<engine::scene::ptr>( );
	
	m_object = resources.get_resource<engine::object::ptr>( );

	render::g_world.get_parameters( ).camera.fov = math::degree_to_radian( 60.0f );

	m_object->update( math::float4x3::identity( ) );
	m_scene->insert( m_object );
	engine::g_world.set_current_scene( m_scene );

	m_camera.create( math::float3( 0.0f ), math::float2( 0.0f ), math::float3( 2.0f, 5.0f, 15.0f ), 0.002f );

	initialize_console( );

	engine::g_world.set_game_ready( );
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
		render::g_world.get_parameters( ).camera.view = m_camera.get_view_matrix( );
	else
	{
		render::ui_batch& batch = render::g_world.get_ui_batch( );
		batch.clear( );
		m_console.render( batch );
	}
}

void world::destroy( )
{
	m_console.destroy( );

	m_console_font->release( );
	m_console_font.reset( );

	resource_system::destroy_resources(
		resource_system::user_callback_task<world, &world::on_resources_destroyed>( this ),
		m_object.get( ), m_scene.get( )
	);

	m_object.reset( );
	m_scene.reset( );
}

void world::on_resources_destroyed( )
{
	engine::g_world.set_game_ready( );
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

		if ( ( k == key::f9 ) && pressed )
		{
			render::g_world.get_parameters( ).draw_statistics = !render::g_world.get_parameters( ).draw_statistics;
		}

		if ( m_console_visible )
			m_console.on_input( );
    }
}

world g_world;

} // namespace game