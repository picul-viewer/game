#ifndef GUARD_RENDER_MESH_MANAGER_H_INCLUDED
#define GUARD_RENDER_MESH_MANAGER_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include "dx_resource.h"
#include "gpu_heap_allocator.h"

namespace render {

class mesh_manager
{
public:
	void create(
		pvoid const in_allocator_memory,
		u32 const in_index_allocator_size,
		u32 const in_vertex_allocator_size,
		u32 const in_index_storage_size,
		u32 const in_vertex_storage_size
	);
	void destroy( );

	u32 create_mesh_index_buffer( u32 const in_index_count );
	u32 create_mesh_vertex_buffer( u32 const in_vertex_count );
	void destroy_mesh_index_buffer( u32 const in_index_offset, u32 const in_index_count );
	void destroy_mesh_vertex_buffer( u32 const in_vertex_offset, u32 const in_vertex_count );

	inline dx_resource const& index_buffer( ) const { return m_index_buffer; }
	inline dx_resource const& vertex_buffer( ) const { return m_vertex_buffer; }
	inline dx_resource const& data_buffer( ) const { return m_vertex_buffer; }

private:
	gpu_heap_allocator m_index_allocator;
	gpu_heap_allocator m_vertex_allocator;
	dx_resource m_index_buffer;
	dx_resource m_vertex_buffer;
	dx_resource m_data_buffer;

};

extern mesh_manager g_mesh_manager;

} // namespace render

#endif // #ifndef GUARD_RENDER_MESH_MANAGER_H_INCLUDED