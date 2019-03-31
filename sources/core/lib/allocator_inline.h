#ifndef GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED
#define GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED

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

#endif // #ifndef GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED