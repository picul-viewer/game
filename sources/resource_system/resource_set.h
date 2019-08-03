#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SET_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SET_H_INCLUDED

#include <types.h>
#include <lib/linear_set.h>
#include "shared_resource_types.h"

namespace resource_system {

template<typename Type, u32 TableSize, typename PoolType>
class resource_set
{
public:
	typedef typename Type::entry_type entry_type;
	typedef typename Type::cook_type cook_type;

public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	bool get( shared_resource_id const in_id, cook_type* const in_cook, Type*& out_resource );

	void remove( Type* const in_resource );

	Type* from_handle( entry_type const in_handle );
	entry_type to_handle( Type* const in_resource );

private:
	typedef lib::linear_set<entry_type, TableSize> table_type;

public:
	enum : uptr {
		table_memory_size = table_type::memory_size,
		allocator_memory_size = PoolType::memory_size,

		memory_size = table_memory_size + allocator_memory_size
	};

private:
	table_type m_set;
	PoolType m_allocator;

};

} // namespace resource_system

using resource_system::resource_set;

#include "resource_set_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SET_H_INCLUDED