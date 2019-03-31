#ifndef GUARD_CORE_DYNAMIC_HEAP_H_INCLUDED
#define GUARD_CORE_DYNAMIC_HEAP_H_INCLUDED

#include <types.h>

class dynamic_heap
{
public:
	void create( pointer const reserved_memory, uptr const size );
	void destroy( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

	void dump( );

private:
	enum : u32 {
		max_allocation_log_size = 31,
		min_allocation_log_size = 5,

		max_allocation_size = 1u << max_allocation_log_size,
		min_allocation_size = 1u << min_allocation_log_size,

		sizes_count = ( max_allocation_log_size - min_allocation_log_size ) * 2 + 1
	};

private:
	static u32 get_desired_size_index_for_query( u32 const size );
	u32 get_size_index_for_query( u32 const size );
	static u32 get_size_index_for_block( u32 const size );

private:
	// List is organized in the next manner:
	// size(2 * i)		= min_allocation_size * 2 ^ ( i / 2 );
	// size(2 * i + 1)	= size( 2 * i ) * 1.5
	u32 m_size_pointers[sizes_count];
	pointer m_memory;
	pointer m_memory_end;
	pointer m_free_pointer;
	u64 m_size_flags;
	u32 m_first_size;

};

#endif // #ifndef GUARD_CORE_DYNAMIC_HEAP_H_INCLUDED