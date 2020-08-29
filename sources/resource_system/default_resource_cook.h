#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED

#include <types.h>
#include <lib/allocator.h>
#include "resource_cook.h"
#include "queried_resources.h"

namespace resource_system {

template<typename Resource, typename ResourceCook>
class default_resource_cook : public resource_cook
{
public:
	template<typename ... Args>
	inline static uptr size( Args ... ) { return sizeof(ResourceCook); }

	void fill_task_info( task_info& in_info )
	{
		in_info.functor = callback_wrapper<&ResourceCook::create_resource>;
		in_info.user_data = this;
		in_info.thread_index = (u32)ResourceCook::create_thread_index;
	}

protected:
	default_resource_cook( ) :
		resource_cook( &destroy_wrapper )
	{ }

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

	static void destroy_wrapper( pointer const in_ptr )
	{
		ResourceCook* const cook = in_ptr;
		cook->~ResourceCook( );
		std_allocator( ).deallocate( in_ptr );
	}

};

template<typename ResourceCook, typename ... Args>
inline ResourceCook* create_cook( Args&& ... args )
{
	uptr const memory_size = ResourceCook::size( lib::forward<Args>( args ) ... );
	ResourceCook* const memory = std_allocator( ).allocate( memory_size );
	new ( memory ) ResourceCook( lib::forward<Args>( args ) ... );
	return memory;
}

} // namespace resource_system

using resource_system::default_resource_cook;
using resource_system::create_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_COOK_H_INCLUDED