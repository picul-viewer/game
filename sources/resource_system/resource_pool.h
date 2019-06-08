#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_H_INCLUDED

#include <types.h>
#include <system/mt_pool.h>
#include "resource_system_types.h"

namespace resource_system {

typedef u32 resource_handle;

template<typename Resource, uptr MemorySize>
class resource_pool
{
public:
	void create( pointer const in_allocated_memory = nullptr );
	void destroy( );

	Resource* allocate( );
	void deallocate( Resource* const in_resource );

	Resource* ptr_from_id( resource_handle const in_handle ) const;
	resource_handle id_from_ptr( Resource* const in_resource ) const;

	bool contains_pointer( pointer const in_ptr ) const;

private:
	sys::mt_pool<Resource, MemorySize> m_pool;

};

} // namespace resource_system

#include "resource_pool_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_POOL_H_INCLUDED