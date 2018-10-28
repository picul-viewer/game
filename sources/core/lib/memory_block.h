#ifndef __core_memory_block_h_included_
#define __core_memory_block_h_included_

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

#endif // #ifndef __core_memory_block_h_included_