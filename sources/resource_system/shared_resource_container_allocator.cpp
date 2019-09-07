#include "shared_resource_container_allocator.h"
#include <lib/allocator.h>
#include <math/math_common.h>

namespace resource_system {
namespace _internal {

pointer shared_resource_container_allocator::allocate_memory_for_container( uptr const in_size )
{
	if ( m_memory == nullptr )
	{
		m_memory = virtual_allocator( ).reserve( nullptr, max_memory_size );
		m_memory_end = m_memory;
	}

	pointer const result = m_memory_end;
	m_memory_end += in_size;
	return result;
}

void shared_resource_container_allocator::create_shared_resource_containers( )
{
	uptr const size_to_commit = math::align_up( m_memory_end - m_memory, Memory_Page_Size );
	virtual_allocator( ).commit( m_memory, size_to_commit );
}

void shared_resource_container_allocator::destroy_shared_resource_containers( )
{
	virtual_allocator( ).release( m_memory );
}

shared_resource_container_allocator s_shared_resource_container_allocator;

} // namespace _internal
} // namespace resource_system