#include "gpu_heap_allocator.h"
#include <lib/algorithms.h>

namespace render {

u32 gpu_heap_allocator::memory_size( u32 const in_ranges_count )
{
	return in_ranges_count * sizeof(range_info);
}

void gpu_heap_allocator::create(
	pvoid const in_ranges_memory,
	u32 const in_ranges_count,
	u32 const in_heap_size
)
{
	ASSERT( in_ranges_memory != nullptr );
	m_free_ranges.create( in_ranges_memory, in_ranges_count );

	m_free_ranges.push_back( { 0, in_heap_size } );
}

u32 gpu_heap_allocator::allocate( u32 const in_size )
{
	ASSERT( m_free_ranges.size( ) != 0, "not ehough space for allocation" );

	range_info* range = nullptr;
	
	lib::for_each( m_free_ranges.begin( ), m_free_ranges.end( ), [&range, in_size]( range_info& r )
	{
		if ( ( r.size >= in_size ) && ( !range || ( range->size > r.size ) ) )
			range = &r;
	} );

	ASSERT( range != nullptr, "not ehough space for allocation" );

	u32 const result = range->offset;

	if ( range->size == in_size )
	{
		if ( m_free_ranges.size( ) == 1 )
			m_free_ranges.clear( );
		else
		{
			lib::swap( *range, m_free_ranges.back( ) );
			m_free_ranges.pop_back( );
		}
	}
	else
	{
		range->offset += in_size;
		range->size -= in_size;
	}

	return result;
}

void gpu_heap_allocator::deallocate( u32 const in_offset, u32 const in_size )
{
	range_info* prev_range = nullptr;
	range_info* next_range = nullptr;

	lib::for_each(
		m_free_ranges.begin( ), m_free_ranges.end( ),
		[&prev_range, &next_range, in_offset, in_size]( range_info& r )
		{
			if ( r.offset + r.size == in_offset )
				prev_range = &r;
			else if ( r.offset == in_offset + in_size )
				next_range = &r;
		}
	);

	if ( prev_range && next_range )
	{
		prev_range->size += in_size + next_range->size;

		lib::swap( *next_range, m_free_ranges.back( ) );
		m_free_ranges.pop_back( );

		return;
	}

	if ( prev_range )
	{
		prev_range->size += in_size;
		return;
	}

	if ( next_range )
	{
		next_range->size += in_size;
		return;
	}

	m_free_ranges.push_back( { in_offset, in_size } );
	return;
}

} // namespace render