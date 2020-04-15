#ifndef GUARD_RENDER_TEXTURE_MANAGER_H_INCLUDED
#define GUARD_RENDER_TEXTURE_MANAGER_H_INCLUDED

#include <types.h>
#include "dx_descriptor_heap.h"
#include "dx_resource.h"
#include "gpu_pool_allocator.h"

namespace render {

class texture_manager
{
public:
	void create( pvoid const in_free_list_memory, u32 const in_elements_count );
	void destroy( );

	u32 create_texture(
		dx_resource const in_texture,
		D3D12_SHADER_RESOURCE_VIEW_DESC const& in_srv_desc
	);
	void destroy_texture( u32 const in_offset );

	inline dx_descriptor_heap const& heap( ) const { return m_descriptors; }

private:
	gpu_pool_allocator m_allocator;
	dx_descriptor_heap m_descriptors;

};

extern texture_manager g_texture_manager;

} // namespace render

#endif // #ifndef GUARD_RENDER_TEXTURE_MANAGER_H_INCLUDED