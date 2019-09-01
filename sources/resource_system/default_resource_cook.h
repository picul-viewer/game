#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED

#include "resource_cook.h"
#include "queried_resources.h"

namespace resource_system {

template<typename Resource, typename ResourceCook>
class default_resource_cook : public resource_cook
{
public:
	void fill_task_info( task_info& in_info )
	{
		in_info.functor = callback_wrapper<&ResourceCook::create_resource>;
		in_info.user_data = this;
		in_info.thread_index = (u32)ResourceCook::create_thread_index;
	}

protected:
	void init( )
	{
		resource_cook::init( &ResourceCook::destroy );
	}

	template<void(ResourceCook::*Callback)( queried_resources& )>
	task_info callback_task( u32 const in_thread_index = engine_thread_count )
	{
		task_info info;
		info.functor = callback_wrapper<Callback>;
		info.user_data = this;
		info.thread_index = in_thread_index;
		return info;
	}

	template<void(ResourceCook::*Callback)( )>
	task_info callback_task( u32 const in_thread_index = engine_thread_count )
	{
		task_info info;
		info.functor = callback_wrapper<Callback>;
		info.user_data = this;
		info.thread_index = in_thread_index;
		return info;
	}

	void finish( Resource* const in_result )
	{
		finish_cook( in_result );
	}

private:
	using resource_cook::finish_cook;

	template<void(ResourceCook::*Callback)( queried_resources& )>
	static void callback_wrapper( pvoid* const in_resources, u32 const in_resource_count, pointer const in_user_data )
	{
		( ( (ResourceCook*)in_user_data )->*Callback )( queried_resources( in_resources, in_resource_count ) );
	}

	template<void(ResourceCook::*Callback)( )>
	static void callback_wrapper( pvoid* const, u32 const, pointer const in_user_data )
	{
		( ( (ResourceCook*)in_user_data )->*Callback )( );
	}

};

} // namespace resource_system

using resource_system::default_resource_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED