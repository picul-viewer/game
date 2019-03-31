#ifndef GUARD_CORE_SEPARATE_ARENA_TEMPORAL_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_SEPARATE_ARENA_TEMPORAL_ALLOCATOR_H_INCLUDED

#include <types.h>
#include "allocator.h"
#include <system/interlocked.h>

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
struct separate_arena_temporal_allocator
{
public:
	void create( );
	void destroy( );
	
	pointer allocate( uptr const size, u32 const thread_id );
	void deallocate( pointer const p, uptr const size );
	
	pointer data( ) const;

private:
	enum : uptr {
		page_size = virtual_allocator::memory_size_helper<PageSize>::value,
		page_count = ThreadMaxCount * PagesPerThread
	};
	
private:
	mt_u32 m_pages_occupancy[page_count];
	union
	{
		struct
		{
			u32 occupancy;
			u32 index;
		};
		u8 padding[Cache_Line];
	} m_active_page_data[ThreadMaxCount];
	pointer m_data;
};

#include "separate_arena_temporal_allocator_inline.h"

#endif // #ifndef GUARD_CORE_SEPARATE_ARENA_TEMPORAL_ALLOCATOR_H_INCLUDED