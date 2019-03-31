#ifndef __core_hash_h_included_
#define __core_hash_h_included_

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

#endif // #ifndef __core_hash_h_included_