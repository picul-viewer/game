#include "mt_heap.h"
#include <macros.h>
#include <math/math_common.h>
#include <intrin.h>

enum { invalid_size_index = 0xFFFFFFFF };

void mt_heap::create( pointer const memory, uptr const size )
{
	ASSERT_CMP( size, <, ( 1ull << representative_size_bits ) * min_allocation_size );
	ASSERT( ( size % min_allocation_size ) == 0 );

	m_memory = memory;
	m_memory_size = (u32)( size / min_allocation_size );
	m_free_pointer = 0;
	m_first_size = 0;

	for ( u32 i = 0; i < sizes_count; ++i )
		m_size_pointers[i] = invalid_size_index;
}

void mt_heap::destroy( )
{ }

// Service data for block.
// This is 16 bytes in size, so that's least possible granularity of the heap.
// This is organized such that there is 4 bytes overhead per allocation.
// Unfortunately this always will be non-aligned by 16 bytes, so sometimes cross cache line.
struct block_data
{
	union
	{
		struct
		{
			// If previous block is allocated, this is invalid.
			// Represent size of previous block.
			mt_u32 prev_size;
			// Always valid.
			// MSB is free flag for previous block, second MSB (2MSB) is free flag for current block, other bits represent size of current block.
			mt_u32 this_size_data;
		};
		mt_u64 size_data;
	};
	// If current block is allocated, these are invalid.
	// This is pointers for double-linked list of the blocks with equal size.
	mt_u32 list_prev_index;
	mt_u32 list_next_index;
};

enum : u32{
	prev_block_free_flag_mask = 0x80000000,
	this_block_free_flag_mask = 0x40000000,
	this_block_size_mask = 0x3FFFFFFF,

	free_pointer_lock_mask = 0x80000000,
	free_pointer_data_mask = 0x7FFFFFFF
};

static inline block_data* get_block_data_for_pointer( pointer p )
{
	return p - sizeof(u32) * 2;
}

u32 mt_heap::get_desired_size_index_for_query( u32 const size_in_cells )
{
	u32 msb_index;
	u8 const result = _BitScanReverse( (unsigned long*)&msb_index, size_in_cells );
	ASSERT( result == 1 );

	if ( ( size_in_cells & ( size_in_cells - 1 ) ) == 0 )
		return msb_index * 2;

	u32 const previous_bit_mask = 1u << ( msb_index - 1 );

	return msb_index * 2 + ( ( size_in_cells & previous_bit_mask ) ? 2 : 1 );
}

u32 mt_heap::get_size_index_for_block( u32 const size_in_cells )
{
	u32 msb_index;
	u8 const result = _BitScanReverse( (unsigned long*)&msb_index, size_in_cells );
	ASSERT( result == 1 );

	u32 const previous_bit_mask = 1u << ( msb_index - 1 );

	return msb_index * 2 + ( ( size_in_cells & previous_bit_mask ) ? 1 : 0 );
}

u32 mt_heap::list_pop_for_query( u32 const size_in_cells, mt_u32*& block_this_size_data_ref, u32& block_this_size_data )
{
	u32 const desired_size_index = get_desired_size_index_for_query( size_in_cells );

	for ( u32 list_index = desired_size_index; list_index < sizes_count; ++list_index )
	{
		u32 size_list_value = m_size_pointers[list_index];

		while ( size_list_value != invalid_size_index )
		{
			if ( size_list_value == invalid_size_index - 1 )
			{
				do
				{
					PAUSE( );
					size_list_value = m_size_pointers[list_index];
				}
				while ( size_list_value == invalid_size_index - 1 );

				continue;
			}

			if ( interlocked_compare_exchange( m_size_pointers[list_index],
				invalid_size_index - 1, size_list_value ) != size_list_value )
			{
				PAUSE( );
				size_list_value = m_size_pointers[list_index];
				continue;
			}

			block_data* const block = get_block_data_for_pointer( m_memory + size_list_value );
			mt_u32& this_size_data_ref = size_list_value ? block->this_size_data : m_first_size;
			u32 const this_size_data = this_size_data_ref;
			bool const block_not_locked = ( this_size_data & this_block_size_mask ) != 0;

			if ( block_not_locked && ( interlocked_compare_exchange( this_size_data_ref,
				this_size_data & ~this_block_size_mask, this_size_data ) == this_size_data ) )
			{
				block_this_size_data_ref = &this_size_data_ref;
				block_this_size_data = this_size_data;
				m_size_pointers[list_index] = block->list_next_index;
				return size_list_value;
			}

			m_size_pointers[list_index] = size_list_value;

			u32 new_size_list_value;

			do
			{
				PAUSE( );
				new_size_list_value = m_size_pointers[list_index];
			}
			while ( size_list_value == new_size_list_value );

			size_list_value = new_size_list_value;
		}
	}

	return invalid_size_index;
}

void mt_heap::list_push( u32 const list_index, u32 const block_index )
{
	u32 size_list_value;

	do
	{
		do
		{
			PAUSE( );
			size_list_value = m_size_pointers[list_index];
		}
		while ( size_list_value == invalid_size_index - 1 );
	}
	while ( interlocked_compare_exchange( m_size_pointers[list_index], invalid_size_index - 1, size_list_value ) != size_list_value );

	if ( size_list_value != invalid_size_index )
	{
		block_data* const next_data = get_block_data_for_pointer( m_memory + size_list_value );
		next_data->list_prev_index = block_index;
	}

	block_data* const data = get_block_data_for_pointer( m_memory + block_index );
	data->list_next_index = size_list_value;
	m_size_pointers[list_index] = block_index;
}

void mt_heap::list_remove( u32 const list_index, u32 const block_index )
{
	u32 size_list_value;

	do
	{
		do
		{
			PAUSE( );
			size_list_value = m_size_pointers[list_index];
		}
		while ( size_list_value == invalid_size_index - 1 );
	}
	while ( interlocked_compare_exchange( m_size_pointers[list_index], invalid_size_index - 1, size_list_value ) != size_list_value );

	ASSERT( size_list_value != invalid_size_index );

	block_data* const data = get_block_data_for_pointer( m_memory + block_index );

	u32 const list_next_index = data->list_next_index;
	u32 const list_prev_index = data->list_prev_index;

	u64 size_flags_unset = 1;

	if ( list_next_index != invalid_size_index )
	{
		block_data* const list_next_data = get_block_data_for_pointer( m_memory + list_next_index );
		list_next_data->list_prev_index = data->list_prev_index;
		size_flags_unset = 0;
	}

	if ( size_list_value != block_index )
	{
		block_data* const list_prev_data = get_block_data_for_pointer( m_memory + list_prev_index );
		list_prev_data->list_next_index = data->list_next_index;
	}

	size_list_value = ( size_list_value != block_index ) ? size_list_value : list_next_index;
	m_size_pointers[list_index] = size_list_value;
}

pointer mt_heap::allocate( uptr size )
{
	// Keep in mind additional 4 bytes for block size and min_allocation_size-byte granularity.
	uptr const allocation_size = math::align_up( size + sizeof(u32), min_allocation_size );
	ASSERT_CMP( allocation_size, <, max_allocation_size );
	u32 const request_size = (u32)( allocation_size / min_allocation_size );

	mt_u32* this_size_data_ref;
	u32 this_size_data;
	u32 const block_index = list_pop_for_query( request_size, this_size_data_ref, this_size_data );

	// If there's suitable block.
	if ( block_index != invalid_size_index )
	{
		// If such block exists, use it to allocate memory.
		cell* const result = m_memory + block_index;
		block_data* const data = get_block_data_for_pointer( result );

		// Get block size properly depending on it's location.
		u32 const block_size = this_size_data & this_block_size_mask;

		ASSERT_CMP( block_size, >=, request_size );

		// Calculate redudant memory size.
		u32 const remainder_size = block_size - request_size;

		// Calculate next block data pointer.
		cell* const next = result + block_size;
		block_data* const next_data = get_block_data_for_pointer( next );

		// Check whether there is a notable redudant memory in the block.
		if ( remainder_size > 0 )
		{
			// Need to split found block into two: first will become allocated ( basis ), second will stay free ( remainder ).

			// Update remainder block data properly.
			cell* const remainder_pointer = result + request_size;
			block_data* const remainder_block_data = get_block_data_for_pointer( remainder_pointer );

			// Lock remainder block.
			remainder_block_data->this_size_data = 0;

			// Update next after remainder block data properly.
			cell* const next = result + block_size;
			block_data* const next_data = get_block_data_for_pointer( next );

			u32 next_prev_size;

			do
			{
				do
				{
					PAUSE( );
					next_prev_size = next_data->prev_size;
				}
				while ( next_prev_size == 0 );
			}
			while ( interlocked_compare_exchange( next_data->prev_size, remainder_size, next_prev_size ) != next_prev_size );

			// Add remainder block to it's size list.
			u32 const remainder_size_index = get_size_index_for_block( remainder_size );
			ASSERT_CMP( remainder_size_index, <, sizes_count );

			u32 const remainder_index = block_index + request_size;

			list_push( remainder_size_index, remainder_index );

			// Unlock remainder block.
			remainder_block_data->this_size_data = remainder_size | this_block_free_flag_mask;
		}
		else	
		{
			// Else, update next block data previous block free flag.
			// Unset MSB.
			u32 next_prev_size;

			do
			{
				do
				{
					PAUSE( );
					next_prev_size = next_data->prev_size;
				}
				while ( next_prev_size == 0 );
			}
			while ( interlocked_compare_exchange( next_data->prev_size, 0, next_prev_size ) != next_prev_size );

			interlocked_and( next_data->this_size_data, ~prev_block_free_flag_mask );
		}

		// Unlock result block.
		interlocked_xor( *this_size_data_ref, this_block_free_flag_mask | request_size );

		return result;
	}

	// Else, allocate block in untouched memory.
	// Lock free pointer.
	u32 free_pointer;
	do
	{
		do
		{
			PAUSE( );
			free_pointer = m_free_pointer;
		}
		while ( ( free_pointer & free_pointer_lock_mask ) != 0 );
	}
	while ( interlocked_compare_exchange( m_free_pointer, free_pointer | free_pointer_lock_mask, free_pointer ) != free_pointer );

	u32 const result_index = free_pointer & free_pointer_data_mask;
	cell* const result = m_memory + result_index;
	ASSERT_CMP( free_pointer, <, m_memory_size );

	// Update next block data ( next block does not exist for now, but may be in the future ).
	// Only previous block free flag should be updated.
	//block_data* const next_data = get_block_data_for_pointer( result + request_size );

	// Unset MSB. (And do not care for 2MSB.)
	// Fill non-zero garbage for size since zero value is lock value.
	//next_data->this_size_data = 0;

	// Depending on whether this is very beginning of the heap memory, update service data properly.
	// Know for sure, that previous block is allocated ( because it's tracked during deallocation,
	// and this block would be merged with untouched memory ), so set previous block free flag to zero.
	// Also this block is allocated, so do not set 2MSB.
	mt_u32& result_this_data = ( result_index != 0 ) ? get_block_data_for_pointer( result )->this_size_data : m_first_size;
	result_this_data = request_size;

	// Unlock free pointer.
	m_free_pointer = free_pointer + request_size;

	return result;
}

void mt_heap::deallocate( pointer p )
{
	ASSERT( ( (pbyte)p >= (pbyte)m_memory ) && ( (pbyte)p < (pbyte)( m_memory + m_free_pointer ) ) );
	ASSERT( ( ( (pbyte)p - (pbyte)m_memory ) % min_allocation_size ) == 0 );

	cell* ptr = (cell*)p;

	block_data* const current_block_data = get_block_data_for_pointer( p );
	mt_u32& this_size_data_ref = ( ptr != m_memory ) ? current_block_data->this_size_data : m_first_size;

	u32 this_size_data;

	do
	{
		do
		{
			PAUSE( );
			this_size_data = this_size_data_ref;
		}
		while ( ( this_size_data & this_block_size_mask ) == 0 );
	}
	while ( interlocked_compare_exchange( this_size_data_ref, this_size_data & ~this_block_size_mask, this_size_data ) != this_size_data );

	// This will be used for size list update.
	cell* free_pointer = ptr;
	mt_u32* free_this_size_data_ref = &this_size_data_ref;

	// Get current block size depending on location.
	u32 const current_block_size = this_size_data & this_block_size_mask;
	// Default free size is current block size.
	u32 free_size = current_block_size;

	// If it's not beginning of the heap memory, try merge the previous block.
	if ( ptr != m_memory )
	{
		// Try to lock previous block, checking every time if it's not allocated.
		union size_data_lock_type
		{
			struct
			{
				u32 prev_size;
				u32 this_size_data;
			};
			u64 size_data;
		};

		size_data_lock_type data;
		data.size_data = current_block_data->size_data;

		while ( this_size_data_ref & prev_block_free_flag_mask )
		{
			if ( data.prev_size == 0 )
			{
				PAUSE( );
				data.size_data = current_block_data->size_data;
				continue;
			}

			size_data_lock_type lock_value;
			lock_value.prev_size = 0;
			lock_value.this_size_data = data.this_size_data;

			if ( interlocked_compare_exchange( current_block_data->size_data, lock_value.size_data, data.size_data ) != data.size_data )
			{
				PAUSE( );
				data.size_data = current_block_data->size_data;
				continue;
			}

			// Lock previous size.
			u32 const previous_size = data.prev_size;
			cell* const previous_block = ptr - previous_size;
			block_data* const previous_data = get_block_data_for_pointer( previous_block );
			mt_u32* const prev_this_size_data_ref = ( previous_block != m_memory ) ? &previous_data->this_size_data : &m_first_size;
			u32 const prev_this_size_data = *prev_this_size_data_ref;

			if ( ( ( prev_this_size_data & this_block_size_mask ) == 0 ) || ( interlocked_compare_exchange( *prev_this_size_data_ref,
				prev_this_size_data & ~this_block_size_mask, prev_this_size_data ) != prev_this_size_data ) )
			{
				current_block_data->prev_size = previous_size;
				PAUSE( );
				data.size_data = current_block_data->size_data;
				continue;
			}

			// Find appropriate size list for previous block.
			ASSERT_CMP( previous_size, <=, max_allocation_size_in_cells );
			u32 const previous_size_index = get_size_index_for_block( previous_size );
			ASSERT_CMP( previous_size_index, <, sizes_count );

			// Remove previous block from it's size list.
			list_remove( previous_size_index, (u32)( previous_block - m_memory ) );

			// Update data for size lists update.
			free_pointer = previous_block;
			free_size = current_block_size + previous_size;
			free_this_size_data_ref = prev_this_size_data_ref;
			break;
		}
	}

	cell* const next_block = ptr + current_block_size;

	// Check whether untouched memory starts right after current block.
	// If untouched memory goes right after current block, merge free block with untouched memory.
	{
		u32 const next_block_index = (u32)( next_block - m_memory );
		u32 const free_block_index = (u32)( free_pointer - m_memory );

		u32 free_pointer = m_free_pointer;

		// If free pointer is locked, there's going to appear new allocation
		// in the end, so this block is certainly not the last, so skip this.
		// If free block is not the last one - also skip.
		while ( ( ( free_pointer & free_pointer_lock_mask ) == 0 ) && ( free_pointer == next_block_index ) )
		{
			// Try to update free pointer.
			if ( interlocked_compare_exchange( m_free_pointer, free_block_index, free_pointer ) == free_pointer )
			{
				// If update is successful - just exit.
				return;
			}

			// Else, repeat all the stuff.
			free_pointer = m_free_pointer;
		}
	}

	block_data* const next_data = get_block_data_for_pointer( next_block );
	// Check that free flag for current block was correct.
	ASSERT( ( next_data->this_size_data & prev_block_free_flag_mask ) == 0 );

	// Lock next block.
	u32 next_this_size_data;
	do
	{
		do
		{
			PAUSE( );
			next_this_size_data = next_data->this_size_data;
		}
		while ( ( next_this_size_data & this_block_size_mask ) == 0 );
	}
	while ( interlocked_compare_exchange( next_data->this_size_data,
		next_this_size_data & ~this_block_size_mask, next_this_size_data ) != next_this_size_data );

	// Check whether next block is free.
	if ( next_this_size_data & this_block_free_flag_mask )
	{
		// If next block is free, remove it from it's size list.
		u32 const next_size = next_this_size_data & this_block_size_mask;
		ASSERT_CMP( next_size, <, max_allocation_size_in_cells );
		u32 const next_size_index = get_size_index_for_block( next_size );
		ASSERT_CMP( next_size_index, <, sizes_count );

		list_remove( next_size_index, (u32)( next_block - m_memory ) );

		// Update data for size lists update.
		free_size += next_size;

		// Access next after next block data to check whether next block is free.
		block_data* const next_after_free_data = get_block_data_for_pointer( next_block + next_size );

		// Update next after next block data properly.
		// Need to update previous block size value.
		// It's fine to update it here since free block this size data is locked.
		u32 next_after_free_prev_size;

		do
		{
			do
			{
				PAUSE( );
				next_after_free_prev_size = next_after_free_data->prev_size;
			}
			while ( next_after_free_prev_size == 0 );
		}
		while ( interlocked_compare_exchange( next_after_free_data->prev_size, free_size,
			next_after_free_prev_size ) != next_after_free_prev_size );
	}
	else
	{
		// Validate previous size value in next block data.
		// Don't need to track prev size locking, since it can't be
		// locked while the whole block is locked by this function.
		next_data->prev_size = free_size;

		// Unlock next block; mark current block as free in next block data.
		// Just do simple assignment, since we know the block size and free flags.
		// It's fine to unlock it here since prev size is updated, and free block is locked.
		ASSERT( ( next_this_size_data & ~this_block_size_mask ) == 0 );
		next_data->this_size_data = next_this_size_data | prev_block_free_flag_mask;
	}

	// Add new free block to it's size list.
	ASSERT_CMP( free_size, <, max_allocation_size_in_cells );
	u32 const free_size_index = get_size_index_for_block( free_size );
	ASSERT_CMP( free_size_index, <, sizes_count );

	u32 const free_index = (u32)( free_pointer - m_memory );
	list_push( free_size_index, free_index );

	// Unlock free block, update free block this size data properly.
	ASSERT( ( free_size & ~this_block_size_mask ) == 0 );
	interlocked_or( *free_this_size_data_ref, free_size | this_block_free_flag_mask );
}

void mt_heap::dump( )
{
	cell* p = m_memory;
	u32 size = m_first_size & this_block_size_mask;

	u32 free_count_in_memory = 0;

	LOG( "=============Memory=============\n" );
	LOG( "% 10s|% 10s|% 10s\n", "index", "state", "size" );
	LOG( "================================\n" );

	while ( p - m_memory != m_free_pointer )
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
		if ( m_size_pointers[i] == invalid_size_index )
			continue;

		u32 const current_size = ( 1ull << ( i / 2 + min_allocation_log_size - 1 ) ) * ( ( i % 2 ) ? 3 : 2 );
		LOG( "size: %d\n", current_size );

		u32 prev_j = invalid_size_index;
		u32 j = m_size_pointers[i];

		do
		{
			block_data* const data = get_block_data_for_pointer( m_memory + j );

			// Do not care about first element prev index.
			if (prev_j != invalid_size_index)
				ASSERT_CMP( data->list_prev_index, ==, prev_j );

			LOG( "%d ", j );
			++free_count_in_lists;

			prev_j = j;
			j = data->list_next_index;
		}
		while ( j != invalid_size_index );
		
		LOG( "\n" );
	}

	LOG( "================================\n\n" );

	ASSERT_CMP( free_count_in_lists, ==, free_count_in_memory );
}