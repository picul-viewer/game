#include "texture_manager.h"
#include "dx.h"

namespace render {

void texture_manager::create( pvoid const in_free_list_memory, u32 const in_elements_count )
{
	ASSERT( in_free_list_memory != nullptr );
	m_allocator.create( in_free_list_memory, in_elements_count, 0 );

	m_descriptors.create( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, in_elements_count );
}

void texture_manager::destroy( )
{
	m_descriptors.destroy( );
}

u32 texture_manager::create_texture( dx_resource const in_texture, D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc )
{
	u32 const result = m_allocator.allocate( );

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descriptors.get( )->GetCPUDescriptorHandleForHeapStart( );
	handle.ptr += result * g_dx.cbv_srv_uav_descriptor_size( );

	g_dx.device( )->CreateShaderResourceView( in_texture.get( ), &in_srv_desc, handle );

	return result;
}

void texture_manager::destroy_texture( u32 const in_offset )
{
	m_allocator.deallocate( in_offset );
}

texture_manager g_texture_manager;

} // namespace render