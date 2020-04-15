#ifndef GUARD_CORE_MEMORY_H_INCLUDED
#define GUARD_CORE_MEMORY_H_INCLUDED

#include <types.h>
#include <macros.h>
#include <memory>

namespace memory
{

inline void set( pvoid ptr, uptr size, u8 value )
{
	ASSERT( ptr );
	memset( ptr, value, size );
}

inline void zero( pvoid ptr, uptr size )
{
	ASSERT( ptr );
	memset( ptr, 0, size );
}

inline void copy( pvoid dest, pcvoid src, uptr size )
{
	ASSERT( dest );
	ASSERT( src );
	memcpy( dest, src, size );
}

inline bool equal( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );
	return memcmp( left, right, size ) == 0;
}

inline bool less( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );
	return memcmp( left, right, size ) < 0;
}

inline bool greater( pcvoid left, pcvoid right, uptr size )
{
	ASSERT( left );
	ASSERT( right );
	return memcmp( left, right, size ) > 0;
}

} // namespace memory

#endif // #ifndef GUARD_CORE_MEMORY_H_INCLUDED