#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_H_INCLUDED

#include <types.h>

#include <lib/linear_set.h>
#include <lib/pool_allocator.h>
#include <system/interlocked.h>

#include "default_resource.h"
#include "shared_resource_ptr.h"
#include "shared_resource_types.h"

namespace resource_system {

template<typename Resource, typename ResourceHandle, u32 TableSize>
class shared_resource_container
{
public:
	typedef typename ResourceHandle::value_type entry_type;

public:
	shared_resource_container( );

	template<typename CookType>
	bool get( shared_resource_id const in_id, CookType* const in_cook, Resource*& out_resource );

	void remove( Resource* const in_resource );

	Resource* from_handle( entry_type const in_handle );
	entry_type to_handle( Resource* const in_resource );

private:
	typedef lib::linear_set<entry_type, TableSize> table_type;
	typedef lib::pool_allocator<sizeof(Resource)> pool_type;

public:
	enum : uptr {
		table_memory_size = table_type::memory_size,
		allocator_memory_size = sizeof(Resource) * TableSize,

		memory_size = table_memory_size + allocator_memory_size
	};

private:
	table_type m_set;
	pool_type m_allocator;

};

template<typename Resource, typename ResourceHandle, u32 MaxInstanceCount>
class shared_resource : public default_resource<Resource>
{
public:
	typedef shared_resource_ptr<Resource> ptr;
	typedef ResourceHandle handle_type;

public:
	u32 add_ref( );
	u32 release( );

	shared_resource_id id( ) const;
	u32 ref_count( ) const;

public:
	typedef shared_resource_container<Resource, ResourceHandle, MaxInstanceCount> container_type;

	static container_type& container( );

private:
	static container_type s_container;

public:
	typedef typename ResourceHandle::value_type handle_value_type;

	static Resource* from_handle( handle_value_type const& in_handle );
	static Resource* from_handle( ResourceHandle const& in_handle );
	static ResourceHandle to_handle( Resource* const in_resource );
	static handle_value_type to_handle_value( Resource* const in_resource );

private:
	template<typename ContainerResource, typename ContainerResourceHandle, u32 ContainerTableSize>
	friend class shared_resource_container;

	template<typename CookResource, typename CookResourceCook>
	friend class shared_resource_cook;

	void init( pointer const in_cook );

	pointer lock_cook_ptr( );
	void unlock_cook_ptr( pointer const in_cook );

	void finish_creation( shared_resource_id const in_id, u32 const in_initial_ref_count );

private:
	enum : u64 {
		unready_bit = 1ull << ( sizeof(u64) * 8 - 1 )
	};

private:
	union
	{
		struct
		{
			shared_resource_id m_id;
			mt_u32 m_ref_count;
		};
		struct
		{
			mt_u64 m_cook_ptr_and_unready_bit;
		};
	};

};

} // namespace resource_system

using resource_system::shared_resource;
using resource_system::shared_resource_container;

#include "shared_resource_inline.h"
#include "shared_resource_container_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_H_INCLUDED