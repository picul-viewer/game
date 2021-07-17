#ifndef GUARD_CORE_MT_HEAP_H_INCLUDED
#define GUARD_CORE_MT_HEAP_H_INCLUDED

#include <types.h>
#include <system/interlocked.h>

class mt_heap
{
public:
	void create( pointer const memory, uptr const size );
	void destroy( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

	void dump( );

private:
	enum : u64 {
		representative_size_bits = 30,
		max_allocation_size_in_cells = 1ull << representative_size_bits,

		min_allocation_log_size = 4,
		max_allocation_log_size = min_allocation_log_size + representative_size_bits,

		min_allocation_size = 1ull << min_allocation_log_size,
		max_allocation_size = 1ull << max_allocation_log_size,

		sizes_count = ( max_allocation_log_size - min_allocation_log_size ) * 2 + 1
	};

private:
	struct cell { u8 unused[min_allocation_size]; };

private:
	static u32 get_desired_size_index_for_query( u32 const size_in_cells );
	static u32 get_size_index_for_block( u32 const size_in_cells );

	u32 list_pop_for_query( u32 const size_in_cells, mt_u32*& block_this_size_data_ref, u32& block_this_size_data );
	void list_push( u32 const list_index, u32 const block_index );
	void list_remove( u32 const list_index, u32 const block_index );

private:
	// List is organized in the next manner:
	// size(2 * i)		= min_allocation_size * 2 ^ ( i / 2 );
	// size(2 * i + 1)	= size( 2 * i ) * 1.5
	mt_u32 m_size_pointers[sizes_count];
	cell* m_memory;
	u32 m_memory_size;
	mt_u32 m_free_pointer;
	mt_u32 m_first_size;

};

#endif // #ifndef GUARD_CORE_MT_HEAP_H_INCLUDED