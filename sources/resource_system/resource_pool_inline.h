#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_INLINE_H_INCLUDED

namespace resource_system {

template<typename Resource, uptr MemorySize>
void resource_pool<Resource, MemorySize>::create( pointer const in_allocated_memory )
{
	m_pool.create( in_allocated_memory );
}

template<typename Resource, uptr MemorySize>
void resource_pool<Resource, MemorySize>::destroy( )
{
	m_pool.destroy( );
}

template<typename Resource, uptr MemorySize>
Resource* resource_pool<Resource, MemorySize>::allocate( )
{
	return m_pool.allocate( );
}

template<typename Resource, uptr MemorySize>
void resource_pool<Resource, MemorySize>::deallocate( Resource* const in_resource )
{
	m_pool.deallocate( in_resource );
}

template<typename Resource, uptr MemorySize>
Resource* resource_pool<Resource, MemorySize>::ptr_from_id( resource_handle const in_handle ) const
{
	return &m_pool[in_handle];
}

template<typename Resource, uptr MemorySize>
resource_handle resource_pool<Resource, MemorySize>::id_from_ptr( Resource* const in_resource ) const
{
	return m_pool.index_of( in_resource );
}

template<typename Resource, uptr MemorySize>
bool resource_pool<Resource, MemorySize>::contains_pointer( pointer const in_ptr ) const
{
	return m_pool.contains_pointer( in_ptr );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_INLINE_H_INCLUDED