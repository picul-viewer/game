#include "resources.h"
#include <lib/memory.h>
#include <resources/resources_path.h>

#include "render.h"

#include "mesh.h"
#include "texture.h"
#include "model_mesh.h"

#include "parameters.h"

namespace render {

void resources::create( )
{
	m_render_allocator.create( 32 * Mb );

	create_resources( );
	create_images( );
}

void resources::destroy( )
{
	destroy_images( );
	destroy_resources( );

	m_render_allocator.destroy( );
}

enum {
	indices_count = 32 * 1024 * 1024,
	vertices_count = 16 * 1024 * 1024
};

void resources::create_resources( )
{
	u32 const indices_ranges_count = 1024;
	u32 const vertices_ranges_count = 1024;

	u32 const texture_descriptors_memory_size = gpu_pool_allocator::memory_size( texture_descriptors_count );
	u32 const indices_memory_size = gpu_heap_allocator::memory_size( indices_ranges_count );
	u32 const vertices_memory_size = gpu_heap_allocator::memory_size( vertices_ranges_count );
	u32 const mesh_objects_memory_size = gpu_pool_allocator::memory_size( mesh_objects_count );
	u32 const transforms_memory_size = transforms_type::memory_size( transform_count );
	u32 const point_lights_memory_size = point_lights_type::memory_size( point_light_count );

	uptr const memory_size =
		texture_descriptors_memory_size +
		indices_memory_size +
		vertices_memory_size +
		mesh_objects_memory_size +
		transforms_memory_size +
		point_lights_memory_size
	;

	pointer const memory = virtual_allocator( ).allocate( memory_size );

	m_allocator_memory = memory;
	pointer m = memory;

	m_texture_descriptor_allocator.create( m, texture_descriptors_count, 0 );
	m += texture_descriptors_memory_size;
	m_mesh_index_allocator.create( m, indices_ranges_count, indices_count );
	m += indices_memory_size;
	m_mesh_vertex_allocator.create( m, vertices_ranges_count, vertices_count );
	m += vertices_memory_size;
	m_mesh_object_allocator.create( m, mesh_objects_count, 0 );
	m += mesh_objects_memory_size;
	m_transforms.create( m, transform_count );
	m += transforms_memory_size;
	m_point_lights.create( m, point_light_count );
	m += point_lights_memory_size;

	ASSERT( m == memory + memory_size );


	m_srv_uav_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, image_srv_uav_count + texture_descriptors_count, true );
	set_dx_name( m_srv_uav_heap, "srv_uav_heap" );
	m_rtv_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, image_rtv_count, false );
	set_dx_name( m_rtv_heap, "rtv_heap" );
	m_dsv_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_DSV, image_dsv_count, false );
	set_dx_name( m_dsv_heap, "dsv_heap" );

	{
		dx_resource::cook cook;
		cook.create_buffer(
			indices_count * sizeof(u16),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
		m_index_buffer.create( cook );
		set_dx_name( m_index_buffer, "index_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			vertices_count * sizeof(math::float3),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
		m_vertex_buffer.create( cook );
		set_dx_name( m_vertex_buffer, "vertex_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			vertices_count * sizeof(gpu::vertex_data),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
		m_vertex_data_buffer.create( cook );
		set_dx_name( m_vertex_data_buffer, "vertex_data_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			mesh_objects_count * sizeof(gpu::mesh_object),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );
		m_mesh_object_buffer.create( cook );
		set_dx_name( m_mesh_object_buffer, "mesh_object_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			sizeof(gpu::constant_buffer),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_constant_buffers[i].create( cook );
			set_dx_name( m_constant_buffers[i], format( "constant_buffer #%d", i ) );
		}
	}
}

void resources::destroy_resources( )
{
	virtual_allocator( ).deallocate( m_allocator_memory );

	m_transforms.destroy( );
	m_point_lights.destroy( );

	m_srv_uav_heap.destroy( );
	m_rtv_heap.destroy( );
	m_dsv_heap.destroy( );
	m_index_buffer.destroy( );
	m_vertex_buffer.destroy( );
	m_vertex_data_buffer.destroy( );
	m_mesh_object_buffer.destroy( );

	for ( u32 i = 0; i < max_frame_delay; ++i )
		m_constant_buffers[i].destroy( );
}

void resources::create_images( )
{
	{
		dx_resource::cook resource_cook;
		resource_cook.create_texture2d(
			DXGI_FORMAT_R32G32_UINT,
			g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1, 1,
			true, false, true, false, false
		);
		resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		resource_cook.set_initial_state( D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE );
		resource_cook.set_clear_value( DXGI_FORMAT_R32G32_UINT, math::float4( 0.0f, 0.0f, 0.0f, 0.0f ) );

		m_images[image_v_buffer_polygon_id].create( resource_cook );
		set_dx_name( m_images[image_v_buffer_polygon_id], "render_target_v_buffer_polygon_id" );

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Format = DXGI_FORMAT_R32G32_UINT;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.PlaneSlice = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

		g_dx.device( )->CreateShaderResourceView( m_images[image_v_buffer_polygon_id], &srv_desc, srv( image_srv_v_buffer_polygon_id ) );

		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
		rtv_desc.Format = DXGI_FORMAT_R32G32_UINT;
		rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;
		rtv_desc.Texture2D.PlaneSlice = 0;

		g_dx.device( )->CreateRenderTargetView( m_images[image_v_buffer_polygon_id], &rtv_desc, rtv( image_rtv_v_buffer_polygon_id ) );
	}

	{
		dx_resource::cook resource_cook;
		resource_cook.create_texture2d(
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1, 1,
			true, true, false, false, false
		);
		resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		resource_cook.set_initial_state( D3D12_RESOURCE_STATE_UNORDERED_ACCESS );

		m_images[image_radiance].create( resource_cook );
		set_dx_name( m_images[image_radiance], "render_target_radiance" );

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.PlaneSlice = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

		g_dx.device( )->CreateShaderResourceView( m_images[image_radiance], &srv_desc, srv( image_srv_radiance ) );

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		uav_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uav_desc.Texture2D.MipSlice = 0;
		uav_desc.Texture2D.PlaneSlice = 0;

		g_dx.device( )->CreateUnorderedAccessView( m_images[image_radiance], nullptr, &uav_desc, uav( image_uav_radiance ) );
	}

	{
		dx_resource::cook resource_cook;
		resource_cook.create_texture2d(
			DXGI_FORMAT_R32_TYPELESS,
			g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1, 1,
			true, false, false, true, false
		);
		resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		resource_cook.set_initial_state( D3D12_RESOURCE_STATE_DEPTH_WRITE );
		resource_cook.set_clear_value( DXGI_FORMAT_D32_FLOAT, 1.0f, 0u );

		m_images[image_depth_buffer].create( resource_cook );
		set_dx_name( m_images[image_depth_buffer], "depth_buffer" );

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
		dsv_desc.Texture2D.MipSlice = 0;
		g_dx.device( )->CreateDepthStencilView( m_images[image_depth_buffer], &dsv_desc, dsv( image_dsv_screen ) );

		dsv_desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		g_dx.device( )->CreateDepthStencilView( m_images[image_depth_buffer], &dsv_desc, dsv( image_dsv_screen_readonly ) );

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.PlaneSlice = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

		g_dx.device( )->CreateShaderResourceView( m_images[image_depth_buffer], &srv_desc, srv( image_srv_depth_buffer ) );
	}

	{
		dx_resource::cook resource_cook;
		resource_cook.create_texture2d(
			DXGI_FORMAT_R16_TYPELESS,
			sun_shadowmap_dimension, sun_shadowmap_dimension, 1, 1,
			true, false, false, true, false
		);
		resource_cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		resource_cook.set_initial_state( D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE );
		resource_cook.set_clear_value( DXGI_FORMAT_D16_UNORM, 1.0f, 0u );

		m_images[image_sun_shadowmap].create( resource_cook );
		set_dx_name( m_images[image_sun_shadowmap], "sun_shadowmap" );

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		dsv_desc.Format = DXGI_FORMAT_D16_UNORM;
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
		dsv_desc.Texture2D.MipSlice = 0;
		g_dx.device( )->CreateDepthStencilView( m_images[image_sun_shadowmap], &dsv_desc, dsv( image_dsv_sun_shadowmap ) );

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Format = DXGI_FORMAT_R16_UNORM;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.PlaneSlice = 0;
		srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

		g_dx.device( )->CreateShaderResourceView( m_images[image_sun_shadowmap], &srv_desc, srv( image_srv_sun_shadowmap ) );
	}

	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
		rtv_desc.Format = g_dx.back_buffer_format;
		rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;
		rtv_desc.Texture2D.PlaneSlice = 0;

		for ( u32 i = 0; i < max_frame_delay; ++i )
			g_dx.device( )->CreateRenderTargetView( g_dx.swap_chain_buffer( i ), &rtv_desc, rtv( image_rtv_output_0 + i ) );
	}
}

void resources::destroy_images( )
{
	for ( u32 i = 0; i < image_count; ++i )
		m_images[i].destroy( );
}

u32 resources::create_texture( dx_resource const in_texture, D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc )
{
	u32 const result = m_texture_descriptor_allocator.allocate( );

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	handle.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * ( image_srv_uav_count + result );

	g_dx.device( )->CreateShaderResourceView( in_texture, &in_srv_desc, handle );

	return result;
}

void resources::destroy_texture( u32 const in_index )
{
	m_texture_descriptor_allocator.deallocate( in_index );
}

u32 resources::create_mesh_index_buffer( u32 const in_index_count )
{
	return m_mesh_index_allocator.allocate( in_index_count );
}

u32 resources::create_mesh_vertex_buffer( u32 const in_vertex_count )
{
	return m_mesh_vertex_allocator.allocate( in_vertex_count );
}

void resources::destroy_mesh_index_buffer( u32 const in_index_offset, u32 const in_index_count )
{
	m_mesh_index_allocator.deallocate( in_index_offset, in_index_count );
}

void resources::destroy_mesh_vertex_buffer( u32 const in_vertex_offset, u32 const in_vertex_count )
{
	m_mesh_vertex_allocator.deallocate( in_vertex_offset, in_vertex_count );
}

u32 resources::create_mesh_object( )
{
	return m_mesh_object_allocator.allocate( );
}

void resources::destroy_mesh_object( u32 const in_index )
{
	m_mesh_object_allocator.deallocate( in_index );
}

D3D12_INDEX_BUFFER_VIEW resources::index_buffer_view( ) const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = m_index_buffer->GetGPUVirtualAddress( );
	ibv.SizeInBytes = indices_count * sizeof(u16);
	ibv.Format = DXGI_FORMAT_R16_UINT;
	return ibv;
}

D3D12_VERTEX_BUFFER_VIEW resources::vertex_buffer_view( ) const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_vertex_buffer->GetGPUVirtualAddress( );
	vbv.SizeInBytes = vertices_count * sizeof(math::float3);
	vbv.StrideInBytes = sizeof(math::float3);
	return vbv;
}

D3D12_VERTEX_BUFFER_VIEW resources::vertex_data_buffer_view( ) const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_vertex_data_buffer->GetGPUVirtualAddress( );
	vbv.SizeInBytes = vertices_count * sizeof(gpu::vertex_data);
	vbv.StrideInBytes = sizeof(gpu::vertex_data);
	return vbv;
}

dx_resource const& resources::image( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, image_count );
	return m_images[in_index];
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::srv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, image_srv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * in_index;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::rtv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, image_rtv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_rtv_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.rtv_descriptor_size( ) * in_index;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::uav( u32 const in_index ) const
{
	ASSERT_CMP( in_index, >=, image_srv_count );
	ASSERT_CMP( in_index, <, image_srv_uav_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * in_index;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::dsv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, image_dsv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_dsv_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.dsv_descriptor_size( ) * in_index;
	return result;
}

resources g_resources;

} // namespace render