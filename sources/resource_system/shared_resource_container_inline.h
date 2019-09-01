#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_INLINE_H_INCLUDED

#include <lib/allocator.h>
#include <lib/hash.h>

namespace resource_system {

template<typename Resource, typename ResourceHandle, u32 TableSize>
void shared_resource_container<Resource, ResourceHandle, TableSize>::create( pointer const in_memory )
{
	pointer const memory =
		( in_memory == nullptr ) ?
		virtual_allocator( ).allocate( memory_size ) :
		in_memory;

	m_set.create( memory );
	m_allocator.create( memory + table_memory_size, allocator_memory_size );
}

template<typename Resource, typename ResourceHandle, u32 TableSize>
void
shared_resource_container<Resource, ResourceHandle, TableSize>::
destroy( )
{
	m_set.destroy( );
}

template<typename Resource, typename ResourceHandle, u32 TableSize>
template<typename CookType>
bool
shared_resource_container<Resource, ResourceHandle, TableSize>::
get( shared_resource_id const in_id, CookType* const in_cook, Resource*& out_resource )
{
	bool found_is_not_ready = false;
	entry_type* entry;

	bool const result = m_set.find_if_or_insert(
		lib::hash( in_id ),
		[this, in_id, in_cook, &found_is_not_ready]( entry_type const entry )
		{
			Resource& resource = *(Resource*)m_allocator[entry - 1];

			// Lock resource and get pointer to cook.
			CookType* const cook = resource.lock_cook_ptr( );

			// If result is null, resource is ready, just perform ordinary checks.
			if ( cook == nullptr )
			{
				// Check if id is the same.
				if ( resource.id( ) != in_id )
					return false;

				u32 const ref_count = resource.add_ref( );

				// If initial reference count is 0 then resource is in the process of destruction.
				if ( ref_count == 1 )
				{
					resource.release( );
					return false;
				}

				return true;
			}

			// Check if id is the same.
			if ( cook->id( ) != in_id )
			{
				resource.unlock_cook_ptr( cook );
				return false;
			}

			cook->add_subscriber( in_cook );

			resource.unlock_cook_ptr( cook );

			found_is_not_ready = true;
			return true;
		},
		[this, in_cook]( )
		{
			Resource* const resource = m_allocator.allocate( );
			new ( resource ) Resource( );
			resource->init( in_cook );
			return (entry_type)( m_allocator.index_of( resource ) + 1 );
		},
		entry
	);

	out_resource = found_is_not_ready ? nullptr : m_allocator[*entry - 1];

	return result;
}

template<typename Resource, typename ResourceHandle, u32 TableSize>
void
shared_resource_container<Resource, ResourceHandle, TableSize>::
remove( Resource* const in_resource )
{
	m_set.remove( to_handle( in_resource ) );
	m_allocator.deallocate( in_resource );
}

template<typename Resource, typename ResourceHandle, u32 TableSize>
Resource*
shared_resource_container<Resource, ResourceHandle, TableSize>::
from_handle( entry_type const in_handle )
{
	return m_allocator[in_handle];
}

template<typename Resource, typename ResourceHandle, u32 TableSize>
typename
shared_resource_container<Resource, ResourceHandle, TableSize>::
entry_type
shared_resource_container<Resource, ResourceHandle, TableSize>::
to_handle( Resource* const in_resource )
{
	return (entry_type)m_allocator.index_of( in_resource );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_INLINE_H_INCLUDED