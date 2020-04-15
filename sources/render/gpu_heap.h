#ifndef GUARD_RENDER_GPU_HEAP_H_INCLUDED
#define GUARD_RENDER_GPU_HEAP_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include "dx12.h"

namespace render {

class gpu_heap
{
public:
	struct block_info
	{
		u32 offset;
		u32 size;
	};

public:
	void create( u64 const in_heap_size, pointer const in_free_blocks_memory, uptr const in_max_free_blocks );
	void destroy( );

	u64 allocate( D3D12_RESOURCE_ALLOCATION_INFO const& in_allocation_info );
	void deallocate( u64 const in_offset, u64 const in_size );

private:
	lib::buffer_array<block_info> m_free_blocks;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_GPU_HEAP_H_INCLUDED