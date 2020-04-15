#ifndef GUARD_RESOURCE_SYSTEM_SINGLETON_RESOURCE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SINGLETON_RESOURCE_H_INCLUDED

#include <types.h>
#include "resource.h"
#include "resource_creator.h"

namespace resource_system {

template<typename Resource>
class singleton_resource : public resource, public resource_creator
{
public:
	void fill_task_info( task_info& in_info )
	{
		in_info.functor = create_wrapper;
		in_info.user_data = this;
		in_info.thread_index = (u32)Resource::create_thread_index;
	}

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
	static void create_wrapper( pvoid* const, u32 const, pointer const in_user_data )
	{
		( (Resource*)in_user_data )->create_resource( );
	}

	static void destroy_wrapper( pvoid* const, u32 const, pointer const in_user_data )
	{
		( (Resource*)in_user_data )->destroy_resource( );
	}

};

} // namespace resource_system

using resource_system::singleton_resource;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SINGLETON_RESOURCE_H_INCLUDED