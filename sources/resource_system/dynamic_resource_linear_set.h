#ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_SET_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_SET_H_INCLUDED

#include <types.h>
#include <system/mt_pool.h>
#include <system/interlocked.h>
#include "resource_system_types.h"

namespace resource_system {

typedef u32 resource_instance_id;
typedef u32 resource_handle;

enum shared_resource_state : u32
{
	shared_resource_state_absent = 0,
	shared_resource_state_allocated,
	shared_resource_state_ready,

	shared_resource_state_count
};

template<typename Resource, uptr TableSize, uptr DataMemorySize, uptr DataPageMemorySize>
class dynamic_resource_linear_set
{
public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	shared_resource_state get( resource_instance_id const in_resource_instance_id, Resource*& out_resource );

	u32 add_ref( Resource* const in_resource ) const;
	u32 release( Resource* const in_resource ) const;

	void on_resource_created( Resource* const in_resource );
	void on_resource_destroyed( Resource* const in_resource );

	Resource* ptr_from_id( resource_handle const in_handle ) const;
	resource_handle id_from_ptr( Resource* const in_resource ) const;

	bool contains_pointer( pointer const in_ptr ) const;

public:
	enum : uptr {
		table_memory_size = TableSize * sizeof(u32),

		memory_size = table_memory_size + DataMemorySize
	};

private:
	enum : u32 { lock_value = 0xFFFFFFFF };
	static u32 lock_cell( mt_u32& in_table_cell );

private:
	sys::mt_dynamic_pool<Resource, DataMemorySize, DataPageMemorySize> m_pool;
	u32* m_table;

};

} // namespace resource_system

#include "dynamic_resource_linear_set_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DYNAMIC_RESOURCE_LINEAR_SET_H_INCLUDED