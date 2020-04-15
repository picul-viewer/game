#include "render_object_mesh_manager.h"
#include "dx.h"

namespace render {

void render_object_mesh_manager::create( pvoid const in_free_list_memory, u32 const in_elements_count )
{
	ASSERT( in_free_list_memory != nullptr );
	m_allocator.create( in_free_list_memory, in_elements_count, 0 );

	struct render_object_mesh_data
	{
		u32 index_buffer_offset;
		u32 vertex_buffer_offset;
		u32 index_count;
		u32 vertex_count;
		u32 transform_index;
		u32 diffuse_texture_index;
		u32 specular_texture_index;
	};

	dx_resource::cook cook;
	cook.create_buffer( in_elements_count * sizeof(render_object_mesh_data), true, false, false, false );
	cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
	cook.set_initial_state(
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	m_data.create( cook );
}

void render_object_mesh_manager::destroy( )
{
	m_data.destroy( );
}

u32 render_object_mesh_manager::create_render_object_mesh( )
{
	u32 const result = m_allocator.allocate( );
}

void render_object_mesh_manager::destroy_render_object_mesh( u32 const in_offset )
{
	m_allocator.deallocate( in_offset );
}

render_object_mesh_manager g_render_object_mesh_manager;

} // namespace render