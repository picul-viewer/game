#ifndef GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED
#define GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED

#include <malloc.h>

template<uptr Alignment>
pointer aligned_std_allocator<Alignment>::allocate( uptr const size )
{
	return _aligned_malloc( size, Alignment );
}

template<uptr Alignment>
void aligned_std_allocator<Alignment>::deallocate( pointer const p )
{
	_aligned_free( p );
}

#endif // #ifndef GUARD_CORE_ALLOCATOR_INLINE_H_INCLUDED