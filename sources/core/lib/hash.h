#ifndef GUARD_CORE_HASH_H_INCLUDED
#define GUARD_CORE_HASH_H_INCLUDED

#include <types.h>

namespace lib {

u16 hash16( pcvoid const data, uptr const size, u16 const initial_value = 0xFFFF );
u32 hash32( pcvoid const data, uptr const size, u32 const initial_value = 0xFFFFFFFF );

template<typename T>
u32 hash( T const& value )
{
	return hash32( &value, sizeof(T) );
}

} // namespace lib

#endif // #ifndef GUARD_CORE_HASH_H_INCLUDED