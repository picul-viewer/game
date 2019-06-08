#ifndef GUARD_CORE_TEMPORAL_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_TEMPORAL_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>

template<u32 PageMaxCount>
struct temporal_allocator
{
public:
	void create( pointer const reserved_memory = nullptr );
	void destroy( );
	
	pointer allocate( uptr const size );
	void deallocate( pointer const p, uptr const size );
	
	pointer data( ) const;

	bool contains_pointer( pointer const p ) const;

private:
	enum : uptr { page_size = 64 * Kb };
	
public:
	enum : uptr { memory_size = page_size * PageMaxCount };

private:
	mt_u32 m_pages_occupancy[PageMaxCount];
	union
	{
		mt_u64 m_active_page_index_and_occupancy;
		struct
		{
			union
			{
				struct
				{
					mt_u16 m_active_page_occupancy_low;
					mt_u16 m_active_page_occupancy_high;
				};
				mt_u32 m_active_page_occupancy;
			};
			mt_u32 m_active_page_index;
		};
		u8 m_padding[Cache_Line];
	};
	pointer m_data;
};

#include "temporal_allocator_inline.h"

#endif // #ifndef GUARD_CORE_TEMPORAL_ALLOCATOR_H_INCLUDED