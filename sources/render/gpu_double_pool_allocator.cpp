#include "gpu_double_pool_allocator.h"
#include <macros.h>

namespace render {

u32 gpu_double_pool_allocator::memory_size( u32 const in_elements_count )
{
	return sizeof(u32) * in_elements_count;
}

void gpu_double_pool_allocator::create(
	pvoid const in_free_list_memory,
	u32 const in_elements_count,
	u32 const in_left_allocated_count,
	u32 const in_right_allocated_count
)
{
	ASSERT( in_free_list_memory != nullptr );

	m_free_lists = (u32*)in_free_list_memory;
	m_left_free_count = 0;
	m_right_free_count = in_elements_count;
	m_left_last_index = in_left_allocated_count;
	m_right_last_index = in_elements_count - in_right_allocated_count;
	m_elements_count = in_elements_count;
}

u32 gpu_double_pool_allocator::allocate_left( )
{
	if ( m_left_free_count == 0 )
	{
		ASSERT( m_left_last_index < m_right_last_index );
		return m_left_last_index++;
	}
	else
		return m_free_lists[--m_left_free_count];
}

u32 gpu_double_pool_allocator::allocate_right( )
{
	if ( m_right_free_count == m_elements_count )
	{
		ASSERT( m_left_last_index < m_right_last_index );
		return --m_right_last_index;
	}
	else
		return m_free_lists[m_right_free_count++];
}

void gpu_double_pool_allocator::deallocate_left( u32 const in_index )
{
	ASSERT( in_index < m_left_last_index );
	m_free_lists[m_left_free_count++] = in_index;
}

void gpu_double_pool_allocator::deallocate_right( u32 const in_index )
{
	ASSERT( in_index >= m_right_last_index );
	m_free_lists[--m_right_free_count] = in_index;
}

void gpu_double_pool_allocator::deallocate( u32 const in_index )
{
	if ( in_index < m_left_last_index )
		m_free_lists[m_left_free_count++] = in_index;
	else
	{
		ASSERT( in_index >= m_right_last_index );
		m_free_lists[--m_right_free_count] = in_index;
	}
}

} // namespace render