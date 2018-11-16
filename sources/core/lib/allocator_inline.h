#ifndef __core_allocator_inline_h_included_
#define __core_allocator_inline_h_included_

#include <malloc.h>

template<uptr alignment>
pointer aligned_std_allocator<alignment>::allocate( uptr size )
{
	return _aligned_malloc( size, alignment );
}

template<uptr alignment>
void aligned_std_allocator<alignment>::deallocate( pointer p )
{
	_aligned_free( p );
}

#endif // #ifndef __core_allocator_inline_h_included_