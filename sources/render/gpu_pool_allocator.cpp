#include "gpu_pool_allocator.h"
#include <lib/allocator.h>

namespace render {

u32 gpu_pool_allocator::memory_size( u32 const in_elements_count )
{
	return sizeof(u32) * in_elements_count;
}

void gpu_pool_allocator::create(
	pvoid const in_free_list_memory,
	u32 const in_elements_count,
	u32 const in_allocated_count
)
{
	ASSERT( in_free_list_memory != nullptr );
	m_free_list.create( in_free_list_memory, in_elements_count );

	m_elements_count = in_elements_count;
	m_last_index = in_allocated_count;
}

u32 gpu_pool_allocator::allocate( )
{
	u32 index;

	if ( m_free_list.size( ) == 0 )
		index = m_last_index++;
	else
	{
		index = m_free_list.back( );
		m_free_list.pop_back( );
	}

	return index;
}

void gpu_pool_allocator::deallocate( u32 const in_index )
{
	m_free_list.push_back( in_index );
}

} // namespace render