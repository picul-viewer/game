#ifndef GUARD_RENDER_GPU_OBJECT_MANAGER_H_INCLUDED
#define GUARD_RENDER_GPU_OBJECT_MANAGER_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include "dx.h"
#include "dx_resource.h"
#include "gpu_double_pool_allocator.h"
#include "gpu_uploader.h"

namespace render {

template<typename T>
class gpu_object_manager
{
public:
	void create( pointer const in_memory, u32 const in_element_count );
	void destroy( );

	u32 allocate_static( );
	u32 allocate_dynamic( );
	void deallocate( u32 const in_handle );
	void get_init_tasks( u32 const in_handle, T& data, lib::buffer_array<gpu_upload_task>& in_tasks );
	void update( u32 const in_handle, T const& data );
	bool need_gpu_update( ) const;
	void update_gpu_task( gpu_upload_task& in_task );

	inline dx_resource const& buffer( u32 const in_index ) const { ASSERT_CMP( in_index, <, max_frame_delay ); return m_buffers[in_index]; }

	static inline u32 memory_size( u32 const in_element_count );

private:
	gpu_double_pool_allocator m_allocator;
	T* m_cpu_data;
	T* m_cpu_data_staging;

	dx_resource m_buffers[max_frame_delay];

	// Current and previuos
	math::u32x4 m_update_bounds;
	u32 m_element_count;

};

} // namespace render

#include "gpu_object_manager_inline.h"

#endif // #ifndef GUARD_RENDER_GPU_OBJECT_MANAGER_H_INCLUDED