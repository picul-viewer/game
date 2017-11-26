#ifndef __core_allocator_inline_h_included_
#define __core_allocator_inline_h_included_

#include <malloc.h>

pointer mem_allocator::allocate( uptr size )
{
	return malloc( size );
}

void mem_allocator::deallocate( pointer p )
{
	free( p );
}


template<uptr alignment>
pointer aligned_mem_allocator<alignment>::allocate( uptr size )
{
	return _aligned_malloc( size, alignment );
}

template<uptr alignment>
void aligned_mem_allocator<alignment>::deallocate( pointer p )
{
	_aligned_free( p );
}

#endif // #ifndef __core_allocator_inline_h_included_
