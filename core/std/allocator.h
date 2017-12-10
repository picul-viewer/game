#ifndef __core_allocator_h_included_
#define __core_allocator_h_included_

#include <core/types.h>

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
	inline pointer reserve( pointer p, uptr size );
	inline pointer commit( pointer p, uptr size );
	inline void decommit( pointer p, uptr size );
	inline void release( pointer p, uptr size );
};

#include "allocator_inline.h"

#endif // #ifndef __core_allocator_h_included_
