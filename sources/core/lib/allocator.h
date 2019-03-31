#ifndef GUARD_CORE_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_ALLOCATOR_H_INCLUDED

#include <types.h>

struct std_allocator
{
	pointer allocate( uptr size );
	void deallocate( pointer p );
};

template<uptr alignment>
struct aligned_std_allocator
{
	inline pointer allocate( uptr size );
	inline void deallocate( pointer p );
};

static inline uptr get_aligned_size( uptr const size, uptr const page_size )
{
	return ( size + ( page_size - 1 ) ) & ( ~( page_size - 1 ) );
}

struct virtual_allocator
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

#define stack_allocate( n ) (pointer)_alloca( n )

struct fake_allocator
{
	fake_allocator( pointer data );

	pointer allocate( uptr size );
	void deallocate( pointer p );

private:
	pointer m_data;

};

#include "allocator_inline.h"

#endif // #ifndef GUARD_CORE_ALLOCATOR_H_INCLUDED
