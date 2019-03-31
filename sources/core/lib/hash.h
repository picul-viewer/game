#ifndef GUARD_CORE_HASH_H_INCLUDED
#define GUARD_CORE_HASH_H_INCLUDED

#include <types.h>

namespace lib {

struct hash16
{
protected:
	static u16 const table[256];

public:
	typedef u16 value_type;

	u16 operator( )( pcvoid data, uptr size, u16 initial_value = 0xFFFF );
};

struct hash32
{
public:
	typedef u32 value_type;

	u32 operator( )( pcvoid data, uptr size, u32 initial_value = 0xFFFFFFFF );
};

} // namespace lib

#endif // #ifndef GUARD_CORE_HASH_H_INCLUDED