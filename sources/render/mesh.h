#ifndef GUARD_RENDER_MESH_H_INCLUDED
#define GUARD_RENDER_MESH_H_INCLUDED

#include <types.h>

#include <resource_system/shared_resource.h>
#include <utils/engine_threads.h>

#include "dx_resource.h"
#include "handles.h"
#include "texture_manager.h"

namespace render {

class mesh : public shared_resource<mesh, mesh_handle, 4096>
{
public:
	friend class mesh_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_main
	};

	static void destroy_resource( mesh* const in_resource );
	
public:
	u32 index_buffer_offset( ) const { return m_index_buffer_offset; }
	u32 vertex_buffer_offset( ) const { return m_vertex_buffer_offset; }
	u32 index_count( ) const { return m_index_count; }
	u32 vertex_count( ) const { return m_vertex_count; }

private:
	u32 m_index_buffer_offset;
	u32 m_vertex_buffer_offset;
	u32 m_index_count;
	u32 m_vertex_count;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_MESH_H_INCLUDED