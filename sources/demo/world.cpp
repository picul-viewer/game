#include "world.h"
#include <engine/world.h>
#include <render/world.h>

#include <lib/allocator.h>

#include <math/math_3d.h>

#include <system/input.h>
#include <system/window_input.h>

#include <utils/resources_path.h>

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

		config cfg( stack_allocate( 1 * Mb ), 1 * Mb );

		binary_config w( cfg );

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

		m_cubes.create( binary_config( cfg ) );
		
		mem_align(16) math::float4x3 world = math::float4x3( math::float4( 1.0f, 0.0f, 0.0f, 0.0f ),
															 math::float4( 0.0f, 1.0f, 0.0f, 0.0f ),
															 math::float4( 0.0f, 0.0f, 1.0f, 0.0f ) );
		m_cubes.update( world );
	}

	m_scene->create( true );
	m_scene->insert_static_object( &m_cubes );
	m_scene->build_static_scene( );

	render::g_world.get_parameters( ).camera.fov = math::degree_to_radian( 60.0f );

	engine::g_world.set_current_scene( m_scene );

	m_camera.create( math::float3( 0.0f ), math::float2( 0.0f ), math::float3( 2.0f, 5.0f, 15.0f ), 0.002f, &m_input_state );
}

void world::update( )
{
	m_camera.update( 0.005f );
	render::g_world.get_parameters( ).camera.view = m_camera.get_view_matrix( );
	Sleep( 5 );
}

void world::destroy( )
{
	m_cubes.destroy( );
	m_scene->destroy( );
}

void world::window_resize( math::u32x2 const& new_dimensions )
{
}

void world::window_activate( bool is_active )
{
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
			engine::g_world.exit( );
		}
    }

	m_input_state.on_message( &data );
}

world g_world;

} // namespace game