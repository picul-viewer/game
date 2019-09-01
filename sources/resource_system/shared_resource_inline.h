#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_INLINE_H_INCLUDED

#include "shared_resource.h"
#include <macros.h>

namespace resource_system {

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
typename shared_resource<Resource, ResourceHandle, MaxInstanceCount>::container_type
	shared_resource<Resource, ResourceHandle, MaxInstanceCount>::s_container;

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
u32 shared_resource<Resource, ResourceHandle, MaxInstanceCount>::add_ref( )
{
	return interlocked_inc( m_ref_count );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
u32 shared_resource<Resource, ResourceHandle, MaxInstanceCount>::release( )
{
	u32 const ref_count = interlocked_dec( m_ref_count );

	if ( ref_count == 0 )
		destroy( );

	return ref_count;
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
shared_resource_id shared_resource<Resource, ResourceHandle, MaxInstanceCount>::id( ) const
{
	return m_id;
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
u32 shared_resource<Resource, ResourceHandle, MaxInstanceCount>::ref_count( ) const
{
	return m_ref_count;
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
typename shared_resource<Resource, ResourceHandle, MaxInstanceCount>::container_type& shared_resource<Resource, ResourceHandle, MaxInstanceCount>::container( )
{
	return s_container;
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
Resource* shared_resource<Resource, ResourceHandle, MaxInstanceCount>::from_handle( ResourceHandle const& in_handle )
{
	return s_container.from_handle( in_handle );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
ResourceHandle shared_resource<Resource, ResourceHandle, MaxInstanceCount>::to_handle( Resource* const in_resource )
{
	return s_container.to_handle( in_resource );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
typename shared_resource<Resource, ResourceHandle, MaxInstanceCount>::handle_value_type shared_resource<Resource, ResourceHandle, MaxInstanceCount>::to_handle_value( Resource* const in_resource )
{
	return s_container.to_handle( in_resource );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
void shared_resource<Resource, ResourceHandle, MaxInstanceCount>::init( pointer const in_cook )
{
	unlock_cook_ptr( in_cook );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
pointer shared_resource<Resource, ResourceHandle, MaxInstanceCount>::lock_cook_ptr( )
{
	u64 const lock_value = (u64)-1;

	u64 data;

	do
	{
		data = m_cook_ptr_and_unready_bit;

		while ( data == lock_value )
		{
			macros::pause( );
			data = m_cook_ptr_and_unready_bit;
		}

		if ( ( data & unready_bit ) == 0 )
			return nullptr;

		data = interlocked_compare_exchange( m_cook_ptr_and_unready_bit, lock_value, data );
	}
	while ( data == lock_value );

	return (pvoid)( data & ( ~unready_bit ) );
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
void shared_resource<Resource, ResourceHandle, MaxInstanceCount>::unlock_cook_ptr( pointer const in_cook )
{
	m_cook_ptr_and_unready_bit = (u64)in_cook | unready_bit;
}

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
void shared_resource<Resource, ResourceHandle, MaxInstanceCount>::finish_creation( shared_resource_id const in_id, u32 const in_initial_ref_count )
{
	m_id = in_id;
	m_ref_count = in_initial_ref_count;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_INLINE_H_INCLUDED