#include "resources.h"

#include <utils/resources_path.h>

#include "mesh.h"
#include "texture.h"
#include "model_mesh.h"

#include "api.h"
#include "parameters.h"

#include "mesh_box.h"
#include "mesh_quad.h"

#include "constant_buffers_constants.h"

#include "ui_batch.h"

namespace render {

void resources::create( )
{
	{
		render_target_view::cook cook;
		cook.set_tex2d_rtv( g_api.backbuffer_pixel_format );
		m_backbuffer.create( g_api.get_backbuffer( ), cook );
	}

	m_depth_buffer.create(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		g_parameters.screen_resolution,
		D3D11_BIND_DEPTH_STENCIL
	);

	create_default_samplers( );
	create_default_constant_buffers( );

	create_shaders( );

	m_render_object_allocator.create( );

	mesh::container( ).create( nullptr );
	texture::container( ).create( nullptr );
	model_mesh::container( ).create( nullptr );

	create_ui_buffers( );

	m_ui_batch.create( );
}

void resources::destroy( )
{
	m_pipeline_state_pool.destroy( );
	
	m_backbuffer.destroy( );
	m_depth_buffer.destroy( );
	
	destroy_default_samplers( );
	destroy_default_constant_buffers( );
	
	destroy_shaders( );

	m_render_object_allocator.destroy( );

	mesh::container( ).destroy( );
	texture::container( ).destroy( );
	model_mesh::container( ).destroy( );
	
	destroy_ui_buffers( );

	m_ui_batch.destroy( );
}

void resources::create_default_samplers( )
{
	u32 sampler_index = 0;

	{
		sampler::cook cook;
		cook.set_sampler(	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
							D3D11_TEXTURE_ADDRESS_WRAP,
							D3D11_TEXTURE_ADDRESS_WRAP,
							D3D11_TEXTURE_ADDRESS_WRAP );
		m_default_samplers[sampler_index].create( cook );

		++sampler_index;
	}

	{
		sampler::cook cook;
		cook.set_sampler(	D3D11_FILTER_MIN_MAG_MIP_POINT,
							D3D11_TEXTURE_ADDRESS_CLAMP,
							D3D11_TEXTURE_ADDRESS_CLAMP,
							D3D11_TEXTURE_ADDRESS_CLAMP );
		m_default_samplers[sampler_index].create( cook );

		++sampler_index;
	}
	
	{
		sampler::cook cook;
		cook.set_sampler(	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
							D3D11_TEXTURE_ADDRESS_CLAMP,
							D3D11_TEXTURE_ADDRESS_CLAMP,
							D3D11_TEXTURE_ADDRESS_CLAMP );
		m_default_samplers[sampler_index].create( cook );

		++sampler_index;
	}

	ASSERT_CMP( sampler_index, ==, default_sampler_type_count );
}

void resources::create_default_constant_buffers( )
{
	u32 constant_buffer_index = 0;
	
	m_default_constant_buffers[constant_buffer_index].create( nullptr, sizeof(per_frame_constants), true );
	++constant_buffer_index;

	m_default_constant_buffers[constant_buffer_index].create( nullptr, sizeof(per_instance_constants), true );
	++constant_buffer_index;

	ASSERT_CMP( constant_buffer_index, ==, default_constant_buffer_type_count );
}

void resources::create_shaders( )
{
#ifdef DEBUG
	char const* root_path = ( g_api.get_feature_level( ) == D3D_FEATURE_LEVEL_11_0 ) ?
		GET_RESOURCE_PATH( "shaders\\debug_5_0" ) :
		GET_RESOURCE_PATH( "shaders\\debug_4_0" );
#else
	char const* root_path = ( g_api.get_feature_level( ) == D3D_FEATURE_LEVEL_11_0 ) ?
		GET_RESOURCE_PATH( "shaders\\release_5_0" ) :
		GET_RESOURCE_PATH( "shaders\\release_4_0" );
#endif // #ifdef DEBUG

	sys::file f;
	f.create( root_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );

	uptr const size = f.size( );
	pvoid const data = virtual_allocator( ).allocate( size );

	f.read( data, size );
	f.destroy( );

	lib::reader r( data, size );

	m_vertex_shader_container.create( r );
	m_pixel_shader_container.create( r );
	m_geometry_shader_container.create( r );
	m_hull_shader_container.create( r );
	m_domain_shader_container.create( r );
	m_compute_shader_container.create( r );

	virtual_allocator( ).deallocate( data );
}

void resources::create_ui_buffers( )
{
	{
		u32 const vertex_size = get_vertex_type_size( 0, vertex_type_ui );

		buffer::cook cook;
		cook.set_vertex_buffer( vertex_size * ui_batch::max_vertices, true );
		m_ui_vertex_buffer.create( cook, nullptr, vertex_size );
	}

	{
		enum { indices_size = sizeof(u16) * ( ui_batch::max_vertices / 4 * 6 ) };

		u16* indices_data = (u16*)_alloca( indices_size );

		for ( u32 i = 0; i < ui_batch::max_vertices / 4; ++i )
		{
			indices_data[i * 6 + 0] = i * 4 + 0;
			indices_data[i * 6 + 1] = i * 4 + 1;
			indices_data[i * 6 + 2] = i * 4 + 3;
			indices_data[i * 6 + 3] = i * 4 + 0;
			indices_data[i * 6 + 4] = i * 4 + 3;
			indices_data[i * 6 + 5] = i * 4 + 2;
		}

		buffer::cook cook;
		cook.set_index_buffer( indices_size );
		m_ui_index_buffer.create( cook, indices_data, DXGI_FORMAT_R16_UINT );
	}
}

void resources::destroy_default_samplers( )
{
	for ( u32 i = 0; i < default_sampler_type_count; ++i )
		m_default_samplers[i].destroy( );
}

void resources::destroy_default_constant_buffers( )
{
	for ( u32 i = 0; i < default_constant_buffer_type_count; ++i )
		m_default_constant_buffers[i].destroy( );
}

void resources::destroy_shaders( )
{
	m_vertex_shader_container.destroy( );
	m_pixel_shader_container.destroy( );
	m_geometry_shader_container.destroy( );
	m_hull_shader_container.destroy( );
	m_domain_shader_container.destroy( );
	m_compute_shader_container.destroy( );
}

void resources::destroy_ui_buffers( )
{
	m_ui_vertex_buffer.destroy( );
	m_ui_index_buffer.destroy( );
}

sampler& resources::get_default_sampler( u32 in_index )
{
	ASSERT_CMP( in_index, <, default_sampler_type_count );
	return m_default_samplers[in_index];
}

constant_buffer& resources::get_default_constant_buffer( u32 in_index )
{
	ASSERT_CMP( in_index, <, default_constant_buffer_type_count );
	return m_default_constant_buffers[in_index];
}

void resources::bind_default_samplers( ) const
{
	m_default_samplers[0].bind_vs( 0, default_sampler_type_count );
	m_default_samplers[0].bind_ps( 0, default_sampler_type_count );
	//m_default_samplers[0].bind_gs( 0, default_sampler_type_count );
	//m_default_samplers[0].bind_hs( 0, default_sampler_type_count );
	//m_default_samplers[0].bind_ds( 0, default_sampler_type_count );
}

void resources::bind_default_constant_buffers( ) const
{
	m_default_constant_buffers[0].bind_vs( 0, default_constant_buffer_type_count );
	m_default_constant_buffers[0].bind_ps( 0, default_constant_buffer_type_count );
	//m_default_constant_buffers[0].bind_gs( 0, default_constant_buffer_type_count );
	//m_default_constant_buffers[0].bind_hs( 0, default_constant_buffer_type_count );
	//m_default_constant_buffers[0].bind_ds( 0, default_constant_buffer_type_count );
}

void resources::bind_default_resources( ) const
{
	bind_default_samplers( );
	bind_default_constant_buffers( );
}


resources g_resources;

} // namespace render