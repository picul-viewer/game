#ifndef __core_allocator_h_included_
#define __core_allocator_h_included_

#include <types.h>

struct mem_allocator
{
	inline pointer allocate( uptr size );
	inline void deallocate( pointer p );
};

template<uptr alignment>
struct aligned_mem_allocator
{
	inline pointer allocate( uptr size );
	inline void deallocate( pointer p );
};

struct virtual_mem_allocator
{
	template<uptr MemorySize>
	struct memory_size_helper
	{
		static const uptr value = ( MemorySize + ( Memory_Page_Size - 1 ) ) & ( ~( Memory_Page_Size - 1 ) );
	};

	pointer reserve( pointer p, uptr size );
	pointer commit( pointer p, uptr size );
	void decommit( pointer p, uptr size );
	void release( pointer p );

	pointer allocate( uptr size );
	void deallocate( pointer p );
};

#include "allocator_inline.h"

#endif // #ifndef __core_allocator_h_included_
