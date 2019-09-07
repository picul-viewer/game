#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_ALLOCATOR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_ALLOCATOR_H_INCLUDED

#include <types.h>

namespace resource_system {

namespace _internal {

class shared_resource_container_allocator
{
public:
	enum : uptr {
		max_memory_size = 64 * Mb
	};

	pointer allocate_memory_for_container( uptr const in_size );
	void create_shared_resource_containers( );
	void destroy_shared_resource_containers( );

private:
	pointer m_memory;
	pointer m_memory_end;

};

extern shared_resource_container_allocator s_shared_resource_container_allocator;

} // namespace _internal

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_CONTAINER_ALLOCATOR_H_INCLUDED