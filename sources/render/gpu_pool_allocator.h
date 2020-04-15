#ifndef GUARD_RENDER_GPU_POOL_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_GPU_POOL_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>

namespace render {

class gpu_pool_allocator
{
public:
	static u32 memory_size( u32 const in_elements_count );

public:
	void create(
		pvoid const in_free_list_memory,
		u32 const in_elements_count,
		u32 const in_allocated_count
	);

	u32 allocate( );
	void deallocate( u32 const in_index );

private:
	lib::buffer_array<u32> m_free_list;
	u32 m_elements_count;
	u32 m_last_index;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_POOL_ALLOCATOR_H_INCLUDED