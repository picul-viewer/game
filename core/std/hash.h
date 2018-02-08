#ifndef __core_hash_h_included_
#define __core_hash_h_included_

#include <core/types.h>

struct hash16
{
protected:
	static u16 const table[256];

public:
	u16 operator( )( pcvoid begin, pcvoid end, u16 initial_value = 0xFFFF );
};

struct hash32
{
protected:
	static u32 const table[256];

public:
	u32 operator( )( pcvoid begin, pcvoid end, u32 initial_value = 0xFFFFFFFF );
};

#endif // #ifndef __core_hash_h_included_