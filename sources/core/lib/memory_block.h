#ifndef GUARD_CORE_MEMORY_BLOCK_H_INCLUDED
#define GUARD_CORE_MEMORY_BLOCK_H_INCLUDED

#include <types.h>

template<uptr Size>
struct mem_align(1) memory_block
{
	u8 data[Size];
	
	template<typename T>
	operator T const&( ) const
	{
		static_assert( sizeof(T) <= Size, "invalid cast" );
		return *(T*)data;
	}

	template<typename T>
	operator T&( )
	{
		static_assert( sizeof(T) <= Size, "invalid cast" );
		return *(T*)data;
	}
};

#endif // #ifndef GUARD_CORE_MEMORY_BLOCK_H_INCLUDED