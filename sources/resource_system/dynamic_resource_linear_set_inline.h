#ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_INLINE_SET_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_INLINE_SET_H_INCLUDED

#include <intrin.h>
#include <macros.h>
#include <lib/algorithms.h>
#include <lib/allocator.h>

namespace resource_system {

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
void dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::create( pointer const in_reserved_memory )
{
	static_assert( DataMemorySize % Memory_Page_Size == 0, "wrong memory size" );
	static_assert( DataPageMemorySize % Memory_Page_Size == 0, "wrong page size" );
	static_assert( DataMemorySize % DataPageMemorySize == 0, "wrong memory/page size" );

	static_assert( sizeof(Resource) < DataPageMemorySize, "too large resource size" );
	static_assert( table_memory_size % Memory_Page_Size == 0, "wrong table size" );

	m_table = ( in_reserved_memory == nullptr ) ? virtual_allocator( ).reserve( nullptr, memory_size ) : in_reserved_memory;
	virtual_allocator( ).commit( m_table, table_memory_size );

	m_pool.create( (pointer)m_table + table_memory_size );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
void dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::destroy( )
{
	virtual_allocator( ).release( m_table );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
u32 dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::lock_cell( mt_u32& in_table_cell )
{
	u32 table_value;

	do
	{
		do
		{
			table_value = in_table_cell;
		}
		while ( table_value == lock_value );

		table_value = interlocked_compare_exchange( in_table_cell, lock_value, table_value );
	}
	while ( table_value == lock_value );

	return table_value;
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
shared_resource_state dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::get( resource_instance_id const in_resource_instance_id, Resource*& out_resource )
{
	u32 const max_distance = ( TableSize < 256 ) ? (u32)TableSize : 255;

	u32 const start_index = _mm_crc32_u32( in_resource_instance_id, 0xFFFFFFFF ) % TableSize;
	u32 index = start_index;

	u32 current_table_value;
	u8 i;

	// Searching for cell with desired id or cell with poor offset.
	for ( i = 1; ; ++i )
	{
		current_table_value = lock_cell( m_table[index] );

		u8 const current_table_offset = (u8)current_table_value;

		// Check for desired id.
		if ( i == current_table_offset )
		{
			u32 const resource_handle = current_table_value >> 8;

			Resource* const current_resource = &m_pool[resource_handle];

			if ( current_resource->instance_id( ) == in_resource_instance_id )
			{
				out_resource = current_resource;
				add_ref( out_resource );

				m_table[index] = current_table_value;

				return ( out_resource->ref_count( ) & 0x80000000 ) ? shared_resource_state_allocated : shared_resource_state_ready;
			}
		}

		// If poor value is found ( current cell offset is higher then expected offset for desired id ), then exit.
		// This is possible, as Robin Hood hasing is used.
		// If exit, do not unlock current cell.
		if ( i > current_table_offset )
			break;

		m_table[index] = current_table_value;
		index = ( index + 1 ) % TableSize;

		ASSERT( i != max_distance, "set is full" );
	}

	// Resource was not found, allocate one.
	Resource* const result = m_pool.allocate( );
	result->instance_id( ) = in_resource_instance_id;
	// Set MSB, as resource is not created yet.
	result->ref_count( ) = 0x80000001;

	u32 const result_handle = m_pool.index_of( result );
	ASSERT_CMP( result_handle, <, 0x00FFFFFF );

	u32 value_to_insert = current_table_value;
	current_table_value = i | ( result_handle << 8 );

	// Insert value, using Robin Hood strategy.
	while ( value_to_insert != 0 )
	{
		u32 const next_index = ( index + 1 ) % TableSize;
		u32 const next_table_value = lock_cell( m_table[next_index] );

		m_table[index] = current_table_value;
		index = next_index;
		current_table_value = next_table_value;

		ASSERT( ( value_to_insert & 0xFF ) != 0xFF, "unable to insert ( too long offset )" );
		++value_to_insert;

		u8 const current_table_offset = (u8)current_table_value;
		u8 const value_to_insert_offset = (u8)value_to_insert;

		if ( current_table_offset < value_to_insert_offset )
			lib::swap( current_table_value, value_to_insert );
	}

	m_table[index] = current_table_value;

	out_resource = result;
	return shared_resource_state_absent;
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
u32 dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::add_ref( Resource* const in_resource ) const
{
	return interlocked_inc( in_resource->ref_count( ) );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
u32 dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::release( Resource* const in_resource ) const
{
	u32 ref_count = interlocked_dec( in_resource->ref_count( ) );

	if ( ref_count != 0 )
		return ref_count;

	// If reference count is zero, search for corresponding record in table.
	u32 const start_index = _mm_crc32_u32( in_resource->instance_id( ), 0xFFFFFFFF ) % TableSize;
	u32 index = start_index;

	u32 const handle = m_pool.index_of( in_resource );
	ASSERT_CMP( handle, <, 0x00FFFFFF );

	u32 desired_table_value = 1 | ( handle << 8 );
	u32 current_table_value;

	while ( true )
	{
		current_table_value = lock_cell( m_table[index] );

		if ( current_table_value == desired_table_value )
			break;

		m_table[index] = current_table_value;
		index = ( index + 1 ) % TableSize;

		ASSERT( ( desired_table_value & 0xFF ) != 0xFF );
		++desired_table_value;
	}

	// Reload reference count, now with corresponding table cell being locked.
	ref_count = in_resource->ref_count( );

	if ( ref_count != 0 )
	{
		m_table[index] = current_table_value;
		return ref_count;
	}

	// Remove resource from table.
	// Shift all cells left by one position until empty cell of cell with perfect offset is found.
	// Block pairs of cells simultaneously to prevent concurrent execution from overrunning the current cell.
	u32 next_index;

	while ( true )
	{
		next_index = ( index + 1 ) % TableSize;
		current_table_value = lock_cell( m_table[next_index] );

		if ( ( current_table_value & 0xFF ) <= 1 )
			break;

		m_table[index] = current_table_value - 1;
		index = next_index;
	}

	m_table[index] = 0;
	m_table[next_index] = current_table_value;

	return 0;
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
void dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::on_resource_created( Resource* const in_resource )
{
	// Unset MSB of reference counter.
	interlocked_and( in_resource->ref_count( ), 0x7FFFFFFF );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
void dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::on_resource_destroyed( Resource* const in_resource )
{
	// Deallocate resource from pool.
	m_pool.deallocate( in_resource );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
Resource* dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::ptr_from_id( resource_handle const in_handle ) const
{
	return &m_pool[in_handle];
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
resource_handle dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::id_from_ptr( Resource* const in_resource ) const
{
	return m_pool.index_of( in_resource );
}

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
bool dynamic_resource_linear_set<Resource, TableSize, DataMemorySize, DataPageMemorySize>::contains_pointer( pointer const in_ptr ) const
{
	return m_pool.contains_pointer( in_ptr );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_INLINE_SET_H_INCLUDED