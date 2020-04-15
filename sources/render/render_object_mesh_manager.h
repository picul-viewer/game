#ifndef GUARD_RENDER_RENDER_OBJECT_MESH_MANAGER_H_INCLUDED
#define GUARD_RENDER_RENDER_OBJECT_MESH_MANAGER_H_INCLUDED

#include <types.h>
#include "dx_resource.h"
#include "gpu_pool_allocator.h"

namespace render {

class render_object_mesh_manager
{
public:
	void create( pvoid const in_free_list_memory, u32 const in_elements_count );
	void destroy( );

	u32 create_render_object_mesh( );
	void destroy_render_object_mesh( u32 const in_offset );

	inline dx_resource const& data( ) const { return m_data; }

private:
	gpu_pool_allocator m_allocator;
	dx_resource m_data;

};

extern render_object_mesh_manager g_render_object_mesh_manager;

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_OBJECT_MESH_MANAGER_H_INCLUDED