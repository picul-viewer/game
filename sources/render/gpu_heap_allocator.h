#ifndef GUARD_RENDER_GPU_HEAP_ALLOCATOR_H_INCLUDED
#define GUARD_RENDER_GPU_HEAP_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>

namespace render {

class gpu_heap_allocator
{
public:
	static u32 memory_size( u32 const in_ranges_count );

public:
	void create(
		pvoid const in_ranges_memory,
		u32 const in_ranges_count,
		u32 const in_heap_size
	);

	u32 allocate( u32 const in_size );
	void deallocate( u32 const in_offset, u32 const in_size );

private:
	struct range_info
	{
		u32 offset;
		u32 size;
	};

private:
	lib::buffer_array<range_info> m_free_ranges;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_HEAP_ALLOCATOR_H_INCLUDED