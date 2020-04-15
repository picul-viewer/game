#include "mesh_manager.h"

namespace render {

void mesh_manager::create(
	pvoid const in_allocator_memory,
	u32 const in_index_allocator_size,
	u32 const in_vertex_allocator_size,
	u32 const in_index_storage_size,
	u32 const in_vertex_storage_size
)
{
	m_index_allocator.create(
		in_allocator_memory,
		in_index_allocator_size, in_index_storage_size
	);
	m_vertex_allocator.create(
		(pbyte)in_allocator_memory + in_index_allocator_size,
		in_vertex_allocator_size, in_vertex_storage_size
	);

	{
		dx_resource::cook cook;
		cook.create_buffer(
			in_index_storage_size * sizeof(u16),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state(
			D3D12_RESOURCE_STATE_INDEX_BUFFER |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_COPY_DEST
		);
		m_index_buffer.create( cook );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			in_vertex_storage_size * sizeof(math::float3),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state(
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_COPY_DEST
		);
		m_vertex_buffer.create( cook );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			in_vertex_storage_size * (sizeof(math::float2) + sizeof(math::float3)),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state(
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_COPY_DEST
		);
		m_data_buffer.create( cook );
	}
}

void mesh_manager::destroy( )
{
	m_index_buffer.destroy( );
	m_vertex_buffer.destroy( );
	m_data_buffer.destroy( );
}

u32 mesh_manager::create_mesh_index_buffer( u32 const in_index_count )
{
	return m_index_allocator.allocate( in_index_count );
}

u32 mesh_manager::create_mesh_vertex_buffer( u32 const in_vertex_count )
{
	return m_vertex_allocator.allocate( in_vertex_count );
}

void mesh_manager::destroy_mesh_index_buffer( u32 const in_index_offset, u32 const in_index_count )
{
	m_index_allocator.deallocate( in_index_offset, in_index_count );
}

void mesh_manager::destroy_mesh_vertex_buffer( u32 const in_vertex_offset, u32 const in_vertex_count )
{
	m_vertex_allocator.deallocate( in_vertex_offset, in_vertex_count );
}

} // namespace render