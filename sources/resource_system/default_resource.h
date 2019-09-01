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
	void fill_destroy_task_info( task_info& in_info )
	{
		in_info.functor = destroy_wrapper;
		in_info.user_data = this;
		in_info.thread_index = (u32)Resource::destroy_thread_index;
	}

	void destroy( )
	{
		task_info info;
		fill_destroy_task_info( info );
		resource::destroy( info );
	}

	void destroy_sync( )
	{
		task_info info;
		fill_destroy_task_info( info );
		resource::destroy_sync( info );
	}

protected:
	using resource::destroy;
	using resource::destroy_sync;

private:
	static void destroy_wrapper( pvoid* const, u32 const, pointer const in_user_data )
	{
		Resource::destroy_resource( in_user_data );
	}

};

} // namespace resource_system

using resource_system::default_resource;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_H_INCLUDED