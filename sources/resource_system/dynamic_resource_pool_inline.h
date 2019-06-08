#ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_POOL_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_POOL_INLINE_H_INCLUDED

namespace resource_system {

template<typename Resource, uptr MemorySize, uptr PageSize>
void dynamic_resource_pool<Resource, MemorySize, PageSize>::create( pointer const in_reserved_memory )
{
	m_pool.create( in_reserved_memory );
}

template<typename Resource, uptr MemorySize, uptr PageSize>
void dynamic_resource_pool<Resource, MemorySize, PageSize>::destroy( )
{
	m_pool.destroy( );
}

template<typename Resource, uptr MemorySize, uptr PageSize>
Resource* dynamic_resource_pool<Resource, MemorySize, PageSize>::allocate( )
{
	return m_pool.allocate( );
}

template<typename Resource, uptr MemorySize, uptr PageSize>
void dynamic_resource_pool<Resource, MemorySize, PageSize>::deallocate( Resource* const in_resource )
{
	m_pool.deallocate( in_resource );
}

template<typename Resource, uptr MemorySize, uptr PageSize>
Resource* dynamic_resource_pool<Resource, MemorySize, PageSize>::ptr_from_id( resource_handle const in_handle ) const
{
	return &m_pool[in_handle];
}

template<typename Resource, uptr MemorySize, uptr PageSize>
resource_handle dynamic_resource_pool<Resource, MemorySize, PageSize>::id_from_ptr( Resource* const in_resource ) const
{
	return m_pool.index_of( in_resource );
}

template<typename Resource, uptr MemorySize, uptr PageSize>
bool dynamic_resource_pool<Resource, MemorySize, PageSize>::contains_pointer( pointer const in_ptr ) const
{
	return m_pool.contains_pointer( in_ptr );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_POOL_INLINE_H_INCLUDED