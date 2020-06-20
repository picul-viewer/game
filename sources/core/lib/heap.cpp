#include "heap.h"
#include <macros.h>
#include <math/math_common.h>
#include <intrin.h>

enum { invalid_size_index = 0xFFFFFFFF };

void heap::create( pointer const memory, uptr const size )
{
	ASSERT_CMP( size, <, ( 1ull << representative_size_bits ) * min_allocation_size );

	m_memory = memory;
	m_memory_end = m_memory + size;
	m_free_pointer = m_memory;
	m_size_flags = 0;

	for ( u32 i = 0; i < sizes_count; ++i )
		m_size_pointers[i] = invalid_size_index;
}

void heap::destroy( )
{ }

// Service data for block.
// This is 16 bytes in size, so that's least possible granularity of the heap.
// This is organized such that there is 4 bytes overhead per allocation.
// Unfortunately this always will be non-aligned by 16 bytes, so sometimes cross cache line.
struct block_data
{
	// If previous block is allocated, this is invalid.
	// Represent size of previous block.
	u32 prev_size;
	// Always valid.
	// MSB is free flag for previous block, second MSB (2MSB) is free flag for current block, other bits represent size of current block.
	u32 this_size_data;
	// If current block is allocated, these are invalid.
	// This is pointers for double-linked list of the blocks with equal size.
	u32 list_prev_index;
	u32 list_next_index;
};

enum : u32{
	prev_block_free_flag_mask = 0x80000000,
	this_block_free_flag_mask = 0x40000000,
	this_block_size_mask = 0x3FFFFFFF
};

static inline block_data* get_block_data_for_pointer( pointer p )
{
	return p - sizeof(u32) * 2;
}

u32 heap::get_desired_size_index_for_query( u32 const size_in_cells )
{
	u32 msb_index;
	u8 const result = _BitScanReverse( (unsigned long*)&msb_index, size_in_cells );
	ASSERT( result == 1 );

	if ( ( size_in_cells & ( size_in_cells - 1 ) ) == 0 )
		return msb_index * 2;

	u32 const previous_bit_mask = 1u << ( msb_index - 1 );

	return msb_index * 2 + ( ( size_in_cells & previous_bit_mask ) ? 2 : 1 );
}

u32 heap::get_size_index_for_query( u32 const size_in_cells )
{
	u32 const desired_size_index = get_desired_size_index_for_query( size_in_cells );
	
	u32 msb_index;
	u8 const result = _BitScanForward64( (unsigned long*)&msb_index, m_size_flags >> desired_size_index );

	return result ? desired_size_index + msb_index : invalid_size_index;
}

u32 heap::get_size_index_for_block( u32 const size_in_cells )
{
	u32 msb_index;
	u8 const result = _BitScanReverse( (unsigned long*)&msb_index, size_in_cells );
	ASSERT( result == 1 );

	u32 const previous_bit_mask = 1u << ( msb_index - 1 );

	return msb_index * 2 + ( ( size_in_cells & previous_bit_mask ) ? 1 : 0 );
}

pointer heap::allocate( uptr size )
{
	// Keep in mind additional 4 bytes for block size and min_allocation_size-byte granularity.
	uptr const allocation_size = math::align_up( size + sizeof(u32), min_allocation_size );
	ASSERT_CMP( allocation_size, <, max_allocation_size );
	u32 const request_size = (u32)( allocation_size / min_allocation_size );

	u32 const request_size_index = get_size_index_for_query( request_size );

	// If such block is not exists.
	if ( request_size_index == invalid_size_index )
	{
		// Allocate block in untouched memory.
		cell* const result = m_free_pointer;
		m_free_pointer += request_size;
		ASSERT_CMP( m_free_pointer, <, m_memory_end );

		// Depending on whether this is very beginning of the heap memory, update service data properly.
		// Know for sure, that previous block is allocated ( because it's tracked during deallocation,
		// and this block would be merged with untouched memory ), so set previous block free flag to zero.
		// Also this block is allocated, so do not set 2MSB.
		if ( result == m_memory )
			m_first_size = request_size;
		else
		{
			block_data* const result_data = get_block_data_for_pointer( result );
			result_data->this_size_data = request_size;
		}

		// Update next block data ( next block does not exist for now, but may be in the future ).
		// Only previous block free flag should be updated.
		block_data* const next_data = get_block_data_for_pointer( result + request_size );

		// Unset MSB. (And do not care for 2MSB.)
		next_data->this_size_data = 0;

		return result;
	}

	// Else, use found block to allocate memory.
	u32& request_size_list = m_size_pointers[request_size_index];

	u32 const result_index = request_size_list;
	cell* const result = m_memory + result_index;

	// Remove found block from size list.
	block_data* const data = get_block_data_for_pointer( result );

	if ( data->list_next_index == result_index )
	{
		request_size_list = invalid_size_index;
		m_size_flags &= ~( 1ull << request_size_index );
	}
	else
	{
		block_data* const list_prev_data = get_block_data_for_pointer( m_memory + data->list_prev_index );
		block_data* const list_next_data = get_block_data_for_pointer( m_memory + data->list_next_index );

		list_prev_data->list_next_index = data->list_next_index;
		list_next_data->list_prev_index = data->list_prev_index;

		request_size_list = data->list_next_index;
	}

	// Get block size properly depending on it's location.
	u32 const block_size = ( result_index == 0 ) ? m_first_size : ( data->this_size_data & this_block_size_mask );
	ASSERT_CMP( block_size, >=, request_size );

	// Calculate redudant memory size.
	u32 const remainder_size = block_size - request_size;

	// Check whether there is a notable redudant memory in the block.
	if ( remainder_size > 0 )
	{
		// Need to split found block into two: first will become allocated ( basis ), second will stay free ( remainder ).

		// Update remainder block data properly.
		cell* const remainder_pointer = result + request_size;
		block_data* const remainder_block_data = get_block_data_for_pointer( remainder_pointer );

		// Remainder goes after basis which is to be allocated for request, so previous block free flag is unset.
		// Remainder is gonna be free, so set 2MSB.
		remainder_block_data->this_size_data = remainder_size | this_block_free_flag_mask;

		// Update next after remainder block data properly.
		cell* const next = result + block_size;
		block_data* const next_data = get_block_data_for_pointer( next );

		next_data->prev_size = remainder_size;

		// Add remainder block to it's size list.
		u32 const remainder_size_index = get_size_index_for_block( remainder_size );
		ASSERT_CMP( remainder_size_index, <, sizes_count );

		u32& remainder_size_list = m_size_pointers[remainder_size_index];
		u32 const remainder_index = result_index + request_size;

		if ( remainder_size_list == invalid_size_index )
		{
			remainder_block_data->list_prev_index = remainder_index;
			remainder_block_data->list_next_index = remainder_index;

			remainder_size_list = remainder_index;

			m_size_flags |= ( 1ull << remainder_size_index );
		}
		else
		{
			u32 const next_index = remainder_size_list;
			block_data* const next_data = get_block_data_for_pointer( m_memory + next_index );

			u32 const prev_index = next_data->list_prev_index;
			block_data* const prev_data = get_block_data_for_pointer( m_memory + prev_index );

			remainder_block_data->list_next_index = next_index;
			remainder_block_data->list_prev_index = prev_index;

			next_data->list_prev_index = remainder_index;
			prev_data->list_next_index = remainder_index;
		}

		// Update basis block size properly depending on it's location.
		if ( result_index == 0 )
			m_first_size = request_size;
		else
		{
			// Store new size, save previous block free flag, unset this block free flag.
			data->this_size_data = ( data->this_size_data & prev_block_free_flag_mask ) | request_size;
		}
	}
	else	
	{
		// Else, update next block data previous block free flag.
		cell* const next = result + block_size;
		block_data* const next_data = get_block_data_for_pointer( next );

		// Unset MSB.
		next_data->this_size_data &= ~prev_block_free_flag_mask;

		// Unset 2MSB for this result block, if block is not located in the beginning of the heap memory.
		if ( result_index != 0 )
			data->this_size_data &= ~this_block_free_flag_mask;
	}

	return result;
}

void heap::deallocate( pointer p )
{
	ASSERT( ( (pbyte)p >= (pbyte)m_memory ) && ( (pbyte)p < (pbyte)m_free_pointer ) );
	ASSERT( ( ( (pbyte)p - (pbyte)m_memory ) % min_allocation_size ) == 0 );
	cell* ptr = (cell*)p;
	
	block_data* const current_block_data = get_block_data_for_pointer( ptr );

	// This will be used for size list update.
	cell* free_pointer = ptr;
	u32 free_size = 0;
	block_data* free_block_data = current_block_data;

	u32 current_block_size = 0;
	
	// Check whether free block is located in the beginning of the heap memory.
	if ( ptr == m_memory )
	{
		// If block is located in the very beginning of the heap memory, than block size is stored in specific place.
		// Also in this case don't need to check whether previous block is empty ( because there is no previous block ).
		current_block_size = m_first_size;
		free_size = current_block_size;
	}
	else
	{
		// Initialize block size from size cell before block.
		current_block_size = current_block_data->this_size_data & this_block_size_mask;

		// Check whether previous block is free.
		if ( current_block_data->this_size_data & prev_block_free_flag_mask )
		{
			// If previous block is free, then need to merge it with current.
			
			u32 const previous_size = current_block_data->prev_size;
			cell* const previous_block = ptr - previous_size;

			// Find appropriate size list for previous block.
			ASSERT_CMP( previous_size, <=, max_allocation_size_in_cells );
			u32 const previous_size_index = get_size_index_for_block( previous_size );
			ASSERT_CMP( previous_size_index, <, sizes_count );
			
			ASSERT( ( m_size_flags & ( 1ull << previous_size_index ) ) != 0 );

			u32& previous_size_list = m_size_pointers[previous_size_index];
			ASSERT( previous_size_list != invalid_size_index );
			
			// Remove previous block from it's size list.
			block_data* const data = get_block_data_for_pointer( previous_block );
				
			if ( data->list_next_index == previous_block - m_memory )
			{
				ASSERT_CMP( previous_size_list, ==, data->list_next_index );
				previous_size_list = invalid_size_index;
				m_size_flags &= ~( 1ull << previous_size_index );
			}
			else
			{
				block_data* const list_prev_data = get_block_data_for_pointer( m_memory + data->list_prev_index );
				block_data* const list_next_data = get_block_data_for_pointer( m_memory + data->list_next_index );

				list_prev_data->list_next_index = data->list_next_index;
				list_next_data->list_prev_index = data->list_prev_index;

				if ( m_memory + previous_size_list == previous_block )
					previous_size_list = data->list_next_index;
			}

			// Update data for size lists update.
			free_pointer = previous_block;
			free_size = current_block_size + previous_size;
			free_block_data = data;
		}
		else
		{
			// Do not update flags in previous block data, because previous block is free and target flag is invalid.

			// Update data for size lists update.
			free_size = current_block_size;
		}
	}

	cell* const next_block = ptr + current_block_size;

	// Check whether untouched memory starts right after current block.
	if ( next_block == m_free_pointer )
	{
		// If untouched memory goes right after current block, merge free block with untouched memory.
		m_free_pointer = free_pointer;

		// If so, don't need to do anything more.
		return;
	}

	block_data* const next_data = get_block_data_for_pointer( next_block );
	u32 const next_size = next_data->this_size_data & this_block_size_mask;

	// Check that free flag for current block was correct.
	ASSERT( ( next_data->this_size_data & prev_block_free_flag_mask ) == 0 );

	// Check whether next block is free.
	if ( next_data->this_size_data & this_block_free_flag_mask )
	{
		// If next block is free, remove it from it's size list.
		ASSERT_CMP( next_size, <, max_allocation_size_in_cells );
		u32 const next_size_index = get_size_index_for_block( next_size );
		ASSERT_CMP( next_size_index, <, sizes_count );

		ASSERT( ( m_size_flags & ( 1ull << next_size_index ) ) != 0 );

		u32& next_size_list = m_size_pointers[next_size_index];
		ASSERT( next_size_list != invalid_size_index );

		if ( next_data->list_next_index == next_block - m_memory )
		{
			ASSERT_CMP( next_size_list, ==, next_data->list_next_index );
			next_size_list = invalid_size_index;
			m_size_flags &= ~( 1ull << next_size_index );
		}
		else
		{
			block_data* const list_prev_data = get_block_data_for_pointer( m_memory + next_data->list_prev_index );
			block_data* const list_next_data = get_block_data_for_pointer( m_memory + next_data->list_next_index );

			list_prev_data->list_next_index = next_data->list_next_index;
			list_next_data->list_prev_index = next_data->list_prev_index;

			if ( m_memory + next_size_list == next_block )
				next_size_list = next_data->list_next_index;
		}

		// Update data for size lists update.
		free_size += next_size;

		// Access next after next block data to check whether next block is free.
		cell* const next_next_block = next_block + next_size;
		block_data* const next_next_data = get_block_data_for_pointer( next_next_block );

		// Update next after next block data properly.
		// Need to update previous block size value.
		next_next_data->prev_size = free_size;
	}
	else
	{
		// Mark current block as free in next block data.
		next_data->this_size_data |= prev_block_free_flag_mask;

		// Validate previous size value in next block data.
		next_data->prev_size = free_size;
	}

	// Add new free block to it's size list.
	ASSERT_CMP( free_size, <, max_allocation_size_in_cells );
	u32 const free_size_index = get_size_index_for_block( free_size );
	ASSERT_CMP( free_size_index, <, sizes_count );

	u32& free_size_list = m_size_pointers[free_size_index];
	u32 const free_index = (u32)( free_pointer - m_memory );

	if ( free_size_list == invalid_size_index )
	{
		free_block_data->list_prev_index = free_index;
		free_block_data->list_next_index = free_index;

		free_size_list = free_index;
		
		m_size_flags |= ( 1ull << free_size_index );
	}
	else
	{
		u32 const next_index = free_size_list;
		block_data* const next_data = get_block_data_for_pointer( m_memory + next_index );

		u32 const prev_index = next_data->list_prev_index;
		block_data* const prev_data = get_block_data_for_pointer( m_memory + prev_index );

		free_block_data->list_next_index = next_index;
		free_block_data->list_prev_index = prev_index;

		next_data->list_prev_index = free_index;
		prev_data->list_next_index = free_index;
	}

	// Update free block data properly.
	// Check whether free block is located in the beginning of the heap memory.
	if ( free_index == 0 )
	{
		m_first_size = free_size;
	}
	else
	{
		// Store new size, set 2MSB, save previous block free flag.
		free_block_data->this_size_data = ( free_block_data->this_size_data & ( ~this_block_size_mask ) ) | free_size | this_block_free_flag_mask;
	}
}

void heap::dump( )
{
	cell* p = m_memory;
	u32 size = m_first_size;

	u32 free_count_in_memory = 0;

	LOG( "=============Memory=============\n" );
	LOG( "% 10s|% 10s|% 10s\n", "index", "state", "size" );
	LOG( "================================\n" );

	while ( p != m_free_pointer )
	{
		block_data* const next_data = get_block_data_for_pointer( p + size );
		bool const this_free = next_data->this_size_data & prev_block_free_flag_mask;
		LOG( "% 10d|% 10s|% 10d\n", (u32)( p - m_memory ), this_free ? "free" : "allocated", size );

		if ( this_free == true )
		{
			ASSERT_CMP( next_data->prev_size, ==, size );
			++free_count_in_memory;
		}

		if ( p != m_memory )
			ASSERT( ( ( get_block_data_for_pointer( p )->this_size_data & this_block_free_flag_mask ) != 0 ) == this_free );

		p += size;
		size = next_data->this_size_data & this_block_size_mask;
	}

	LOG( "================================\n" );
	
	u32 free_count_in_lists = 0;

	LOG( "===========Size lists===========\n" );

	for ( u32 i = 0; i < sizes_count; ++i )
	{
		if ( ( m_size_flags & ( 1ull << i ) ) == 0 )
		{
			ASSERT( m_size_pointers[i] == invalid_size_index );
			continue;
		}

		ASSERT( m_size_pointers[i] != invalid_size_index );

		u32 const current_size = ( 1ull << ( i / 2 + min_allocation_log_size - 1 ) ) * ( ( i % 2 ) ? 3 : 2 );
		LOG( "size: %d\n", current_size );

		u32 const e = m_size_pointers[i];
		u32 j = e;

		do
		{
			LOG( "%d ", j );

			u32 const next_j = get_block_data_for_pointer( m_memory + j )->list_next_index;

			ASSERT_CMP( get_block_data_for_pointer( m_memory + next_j )->list_prev_index, ==, j );

			++free_count_in_lists;

			j = next_j;
		}
		while ( j != e );
		
		LOG( "\n" );
	}

	LOG( "================================\n\n" );

	ASSERT_CMP( free_count_in_lists, ==, free_count_in_memory );
}