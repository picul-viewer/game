#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_H_INCLUDED

#include <types.h>
#include "resource.h"
#include "default_resource_ptr.h"

namespace resource_system {

template<typename Resource>
class default_resource : public resource
{
public:
	typedef default_resource_ptr<Resource> ptr;

public:
	void delete_resource( )
	{
		query_info info;
		info.this_ptr = this;
		info.task.functor = destroy_wrapper;
		info.task.thread_index = (u32)Resource::destroy_thread_index;
		resource::delete_resource( info );
	}

private:
	static void destroy_wrapper( pointer const in_this_ptr, queried_resources& )
	{
		Resource::destroy( in_this_ptr );
	}

};

} // namespace resource_system

using resource_system::default_resource;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_H_INCLUDED