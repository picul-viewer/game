#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED

#include "resource_cook.h"

namespace resource_system {

template<typename Resource, typename ResourceCook>
class default_resource_cook : public resource_cook
{
public:
	void fill_query( query_info& in_info )
	{
		in_info.this_ptr = this;
		in_info.task.functor = callback<&ResourceCook::create_resource>;
		in_info.task.thread_index = (u32)ResourceCook::create_thread_index;
	}

protected:
	template<typename ResourceType, typename ResourceHandleType, u32 MaxInstanceCount>
	friend class shared_resource;

	void init( )
	{
		resource_cook::init( &ResourceCook::destroy );
	}

	template<void(ResourceCook::*Callback)( queried_resources& )>
	static void callback( pointer const in_this_ptr, queried_resources& resources )
	{
		( ( (ResourceCook*)in_this_ptr )->*Callback )( resources );
	}

	template<void(ResourceCook::*Callback)( )>
	static void callback( pointer const in_this_ptr, queried_resources& )
	{
		( ( (ResourceCook*)in_this_ptr )->*Callback )( );
	}
};

} // namespace resource_system

using resource_system::default_resource_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED