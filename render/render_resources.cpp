#include "render_resources.h"

#include "render_api.h"
#include "render_parameters.h"

#include "mesh_box.h"
#include "mesh_quad.h"

#include "constant_buffers_constants.h"

namespace render {

void resources::create( )
{
	{
		render_target_view::cook cook;
		cook.set_tex2d_rtv( g_api.backbuffer_pixel_format );
		m_backbuffer.create( g_api.get_backbuffer( ), cook );
	}

	m_depth_buffer.create( DXGI_FORMAT_D24_UNORM_S8_UINT, g_parameters->screen_resolution, D3D11_BIND_DEPTH_STENCIL );

	create_default_samplers( );
	create_default_meshes( );
	create_default_constant_buffers( );

	m_vertex_shader_container.create( );
	m_pixel_shader_container.create( );
	m_geometry_shader_container.create( );
	m_hull_shader_container.create( );
	m_domain_shader_container.create( );
	m_compute_shader_container.create( );

	static const uptr mesh_pool_hash_table_size = 256;
	static const uptr texture_pool_hash_table_size = 256;

	m_mesh_pool.create( mesh_pool_hash_table_size );
	m_texture_pool.create( texture_pool_hash_table_size );
}

void resources::destroy( )
{
	m_pipeline_state_pool.destroy( );
	
	m_backbuffer.destroy( );
	m_depth_buffer.destroy( );
	
	destroy_default_samplers( );
	destroy_default_meshes( );
	destroy_default_constant_buffers( );
	
	m_vertex_shader_container.destroy( );
	m_pixel_shader_container.destroy( );
	m_geometry_shader_container.destroy( );
	m_hull_shader_container.destroy( );
	m_domain_shader_container.destroy( );
	m_compute_shader_container.destroy( );

	m_mesh_pool.destroy( );
	m_texture_pool.destroy( );
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

	ASSERT( sampler_index == default_sampler_type_count );
}

void resources::create_default_meshes( )
{
	const u32 stride = sizeof(math::float3);

	u32 mesh_index = 0;
	
	// Box
	{
		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(box::vertices) );
		m_default_meshes[mesh_index].create_vertex_buffer( 0, vertex_cook, box::vertices, stride );

		buffer::cook index_cook;
		vertex_cook.set_index_buffer( sizeof(box::indices) );
		m_default_meshes[mesh_index].create_index_buffer( index_cook, box::indices, DXGI_FORMAT_R16_UINT );

		m_default_meshes[mesh_index].set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_default_meshes[mesh_index].set_dimensions( box::index_count, 0 );

		++mesh_index;
	}

	// Quad
	{
		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(quad::vertices) );
		m_default_meshes[mesh_index].create_vertex_buffer( 0, vertex_cook, quad::vertices, stride );

		buffer::cook index_cook;
		vertex_cook.set_index_buffer( sizeof(quad::indices) );
		m_default_meshes[mesh_index].create_index_buffer( index_cook, quad::indices, DXGI_FORMAT_R16_UINT );

		m_default_meshes[mesh_index].set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_default_meshes[mesh_index].set_dimensions( quad::index_count, 0 );

		++mesh_index;
	}

	ASSERT( mesh_index == default_mesh_type_count );
}

void resources::create_default_constant_buffers( )
{
	u32 constant_buffer_index = 0;
	
	m_default_constant_buffers[constant_buffer_index].create( nullptr, sizeof(per_frame_constants), true );
	++constant_buffer_index;

	m_default_constant_buffers[constant_buffer_index].create( nullptr, sizeof(per_instance_constants), true );
	++constant_buffer_index;

	ASSERT( constant_buffer_index == default_constant_buffer_type_count );
}

void resources::destroy_default_samplers( )
{
	for ( u32 i = 0; i < default_sampler_type_count; ++i )
		m_default_samplers[i].destroy( );
}

void resources::destroy_default_meshes( )
{
	for ( u32 i = 0; i < default_mesh_type_count; ++i )
		m_default_meshes[i].destroy( );
}

void resources::destroy_default_constant_buffers( )
{
	for ( u32 i = 0; i < default_constant_buffer_type_count; ++i )
		m_default_constant_buffers[i].destroy( );
}

sampler& resources::get_default_sampler( u32 in_index )
{
	ASSERT( in_index < default_sampler_type_count );
	return m_default_samplers[in_index];
}

mesh& resources::get_default_mesh( u32 in_index )
{
	ASSERT( in_index < default_mesh_type_count );
	return m_default_meshes[in_index];
}

constant_buffer& resources::get_default_constant_buffer( u32 in_index )
{
	ASSERT( in_index < default_constant_buffer_type_count );
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