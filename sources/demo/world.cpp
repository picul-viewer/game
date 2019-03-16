#include "world.h"
#include <engine/world.h>
#include <render/world.h>

#include <lib/allocator.h>
#include <lib/dynamic_writer.h>

#include <math/math_3d.h>

#include <system/input.h>
#include <system/window_input.h>

#include <utils/resources_path.h>

#include <render/ui_batch.h>

#include <macros.h>
#include <Windows.h>

namespace game {

const u32 box_index_count = 36;

const u16 box_indices[box_index_count] = {
	 0,  1,  3,  0,  3,  2,
	 4,  5,  7,  4,  7,  6,
	 8,  9, 11,  8, 11, 10,
	12, 13, 15, 12, 15, 14,
	16, 17, 19, 16, 19, 18,
	20, 21, 23, 20, 23, 22
};

const float box_vertices[] = {
	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	 1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	 
	-1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f,
	 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f,
	-1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f,
	 
	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	 
	-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 
	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	 
	 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, -1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f
};

void world::create( )
{
	m_scene = engine::g_world.create_scene( );

	{
		float const dimensions = 9.0f;

		dynamic_writer w;
		w.create( );

		// Create render object.
		w.write( true );

		// One render object.
		w.write<u16>( 1000 );

		for ( float x = -dimensions; x <= dimensions; x += 2.0f )
		for ( float y = -dimensions; y <= dimensions; y += 2.0f )
		for ( float z = -dimensions; z <= dimensions; z += 2.0f )
		{
			// Mesh render object.
			w.write<u8>( 0 );

			// Provide render model config.
			w.write<bool>( true );

			// Render model AABB min/max.'
			w.write<math::float3>( math::float3( -1.0f, -1.0f, -1.0f ) );
			w.write<math::float3>( math::float3(  1.0f,  1.0f,  1.0f ) );

			// Read mesh from this config.
			w.write<u8>( 2 );

			// Index data, here just indices count.
			w.write<u32>( box_index_count );

			// Indices.
			w.write_data( box_indices, sizeof(box_indices) );

			// Vertex buffer size.
			w.write<u32>( sizeof(box_vertices) );

			// Vertices.
			w.write_data( box_vertices, sizeof(box_vertices) );

			// Texture paths.
			w.write<u8>( 1 );
			w.write<pcstr>( GET_RESOURCE_PATH( "textures\\diffuse.dds" ) );
			w.write<u8>( 1 );
			w.write<pcstr>( GET_RESOURCE_PATH( "textures\\specular.dds" ) );

			// Mesh object transform.
			w.write<math::float4x3>(
				math::float4x3(
					math::combine_transforms(
						math::matrix_scale( math::float3( 0.6f, 0.6f, 0.6f ) ),
						math::matrix_translation( math::float3( x, y, z ) )
					)
				)
			);
		}

		m_cubes.create( reader( w.data( ), w.size( ) ) );
		
		mem_align(16) math::float4x3 world = math::float4x3( math::float4( 1.0f, 0.0f, 0.0f, 0.0f ),
															 math::float4( 0.0f, 1.0f, 0.0f, 0.0f ),
															 math::float4( 0.0f, 0.0f, 1.0f, 0.0f ) );
		m_cubes.update( world );
		
		w.destroy( );
	}

	m_scene->create( true );
	m_scene->insert_static_object( &m_cubes );
	m_scene->build_static_scene( );

	render::g_world.get_parameters( ).camera.fov = math::degree_to_radian( 60.0f );

	engine::g_world.set_current_scene( m_scene );

	m_camera.create( math::float3( 0.0f ), math::float2( 0.0f ), math::float3( 2.0f, 5.0f, 15.0f ), 0.002f );

	initialize_console( );
}

void world::initialize_console( )
{
	m_console_visible = false;

	pcstr const font_path = GET_RESOURCE_PATH( "configs\\fonts\\console.cfg" );
	m_console.create( font_path, 16 );

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
	float const elapsed_time = m_ticker.tick( );
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
	m_cubes.destroy( );
	m_scene->destroy( );
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