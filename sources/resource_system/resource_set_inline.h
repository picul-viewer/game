#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SET_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SET_INLINE_H_INCLUDED

#include <lib/allocator.h>
#include <lib/hash.h>

template<typename Type, u32 TableSize, typename PoolType>
void resource_system::resource_set<Type, TableSize, PoolType>::create( pointer const in_reserved_memory )
{
	pointer const memory =
		( in_reserved_memory == nullptr ) ?
		virtual_allocator( ).allocate( memory_size ) :
		in_reserved_memory;

	virtual_allocator( ).commit( in_reserved_memory, table_memory_size );

	m_set.create( in_reserved_memory );
	m_allocator.create( in_reserved_memory + table_memory_size );
}

template<typename Type, u32 TableSize, typename PoolType>
void resource_system::resource_set<Type, TableSize, PoolType>::destroy( )
{
	m_set.destroy( );
}

template<typename Type, u32 TableSize, typename PoolType>
bool resource_system::resource_set<Type, TableSize, PoolType>::get( shared_resource_id const in_id, cook_type* const in_cook, Type*& out_resource )
{
	bool found_is_not_ready = false;
	entry_type entry;

	bool const result = m_set.find_if_or_insert(
		lib::hash( in_id ),
		[&m_allocator, in_id, in_cook, &found_is_not_ready]( entry_type const entry )
		{
			Type& const resource = m_allocator[entry - 1];

			// Lock resource and get pointer to cook.
			cook_type* const cook = resource.lock_cook_ptr( );

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
				resource->unlock_cook_ptr( cook );
				return false;
			}

			cook->add_subscriber( in_cook );

			resource->unlock_cook_ptr( cook );

			found_is_not_ready = true;
			return true;
		},
		[&m_allocator, in_cook]( )
		{
			Type* const resource = m_allocator.allocate( sizeof(Type) );
			resource->init( in_cook );
			return m_allocator.index_of( resource ) + 1;
		},
		entry
	);

	out_resource = found_is_not_ready ? nullptr : m_allocator[entry - 1];

	return result;
}

template<typename Type, u32 TableSize, typename PoolType>
void resource_system::resource_set<Type, TableSize, PoolType>::remove( Type* const in_resource )
{
	m_set.remove( in_resource );
	m_allocator.deallocate( in_resource );
}

template<typename Type, u32 TableSize, typename PoolType>
Type* resource_system::resource_set<Type, TableSize, PoolType>::from_handle( entry_type const in_handle )
{
	return &m_allocator[in_handle];
}

template<typename Type, u32 TableSize, typename PoolType>
typename resource_system::resource_set<Type, TableSize, PoolType>::entry_type resource_system::resource_set<Type, TableSize, PoolType>::to_handle( Type* const in_resource )
{
	return (entry_type)m_allocator.index_of( in_resource );
}

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SET_INLINE_H_INCLUDED