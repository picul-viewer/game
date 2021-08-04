#include "resources.h"
#include <lib/memory.h>
#include <resources/resources_path.h>

#include "render.h"

#include "mesh.h"
#include "texture.h"
#include "model_mesh.h"

#include "parameters.h"

namespace render {

void resources::create( u32 const in_render_srv_count, u32 const in_render_rtv_count,
	u32 const in_render_uav_count, u32 const in_render_dsv_count )
{
	m_render_srv_uav_count = in_render_srv_count + in_render_uav_count;
	m_render_srv_count = in_render_srv_count;
	m_render_rtv_count = in_render_rtv_count;
	m_render_uav_count = in_render_uav_count;
	m_render_dsv_count = in_render_dsv_count;

	m_descriptor_ranges[0].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_images_space, m_render_srv_count );
	m_descriptor_ranges[1].create( D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, resources::hlsl_images_space, m_render_uav_count );
	m_descriptor_ranges[2].create( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, resources::hlsl_textures_space, resources::texture_descriptors_count );

	m_render_allocator.create( 32 * Mb );

	create_resources( );
}

void resources::destroy( )
{
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


	m_srv_uav_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_render_srv_uav_count + texture_descriptors_count, true );
	set_dx_name( m_srv_uav_heap, "srv_uav_heap" );
	m_rtv_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_render_rtv_count, false );
	set_dx_name( m_rtv_heap, "rtv_heap" );
	m_dsv_heap.create( D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_render_dsv_count, false );
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
}

u32 resources::create_texture( dx_resource const in_texture, D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc )
{
	u32 const result = m_texture_descriptor_allocator.allocate( );

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	handle.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * ( m_render_srv_uav_count + result );

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

void resources::create_srv( u32 const in_index, dx_resource const in_resource, dx_srv_cook const& in_cook )
{
	in_cook.build( in_resource, srv( in_index ) );
}

void resources::create_rtv( u32 const in_index, dx_resource const in_resource, dx_rtv_cook const& in_cook )
{
	in_cook.build( in_resource, rtv( in_index ) );
}

void resources::create_uav( u32 const in_index, dx_resource const in_resource, dx_uav_cook const& in_cook )
{
	in_cook.build( in_resource, uav( in_index ) );
}

void resources::create_uav( u32 const in_index, dx_resource const in_resource, dx_resource const in_counter_buffer, dx_uav_cook const& in_cook )
{
	in_cook.build( in_resource, in_counter_buffer, uav( in_index ) );
}

void resources::create_dsv( u32 const in_index, dx_resource const in_resource, dx_dsv_cook const& in_cook )
{
	in_cook.build( in_resource, dsv( in_index ) );
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::srv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, m_render_srv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * in_index;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::rtv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, m_render_rtv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_rtv_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.rtv_descriptor_size( ) * in_index;
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::uav( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, m_render_uav_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_srv_uav_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.cbv_srv_uav_descriptor_size( ) * ( m_render_srv_count + in_index );
	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE resources::dsv( u32 const in_index ) const
{
	ASSERT_CMP( in_index, <, m_render_dsv_count );
	D3D12_CPU_DESCRIPTOR_HANDLE result = m_dsv_heap->GetCPUDescriptorHandleForHeapStart( );
	result.ptr += g_dx.dsv_descriptor_size( ) * in_index;
	return result;
}

void resources::serialize_descriptor_table_binding( dx_root_signature::root_parameter& in_parameter, D3D12_SHADER_VISIBILITY const in_visibility )
{
	in_parameter.create_descriptor_table( m_descriptor_ranges, (u32)array_size( m_descriptor_ranges ), in_visibility );
}

resources g_resources;

} // namespace render