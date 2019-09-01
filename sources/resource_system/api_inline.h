#ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED

#include <lib/allocator.h>
#include "resource_cook.h"

template<typename ... Cooks>
void resource_system::create_resources(
	task_info const& in_callback,
	Cooks* ... in_cooks
)
{
	u32 const count = type_count<Cooks ...>::value;
	task_info* const tasks = stack_allocate( count * sizeof(task_info) );

	gather_task_info( gather_create_resource_task_info( ), tasks, in_cooks ... );

	create_resources( tasks, count, in_callback );
}

template<typename ... Resources>
void resource_system::destroy_resources(
	task_info const& in_callback,
	Resources* ... in_resources
)
{
	u32 const count = type_count<Resources ...>::value;
	task_info* const tasks = stack_allocate( count * sizeof(task_info) );

	gather_task_info( gather_destroy_resource_task_info( ), tasks, in_resources ... );

	destroy_resources( tasks, count, in_callback );
}

template<void ( *Callback )( queried_resources&, pointer const )>
task_info resource_system::user_callback_task( pointer const in_user_data, u32 const in_thread_index )
{
	task_info info;
	info.functor = []( pvoid* const in_resources, u32 const in_resource_count, pointer const in_user_data )
	{
		Callback( queried_resources( in_resources, in_resource_count ), in_user_data );
	};
	info.user_data = in_user_data;
	info.thread_index = in_thread_index;
	return info;
}

template<typename ThisType, void ( ThisType::*Callback )( queried_resources& )>
task_info resource_system::user_callback_task( pointer const in_this_ptr, u32 const in_thread_index )
{
	task_info info;
	info.functor = []( pvoid* const in_resources, u32 const in_resource_count, pointer const in_user_data )
	{
		( ( (ThisType*)in_user_data )->*Callback )( queried_resources( in_resources, in_resource_count ) );
	};
	info.user_data = in_this_ptr;
	info.thread_index = in_thread_index;
	return info;
}

template<void ( *Callback )( pointer const )>
task_info resource_system::user_callback_task( pointer const in_user_data, u32 const in_thread_index )
{
	task_info info;
	info.functor = []( pvoid* const, u32 const, pointer const in_user_data )
	{
		Callback( in_user_data );
	};
	info.user_data = in_user_data;
	info.thread_index = in_thread_index;
	return info;
}

template<typename ThisType, void ( ThisType::*Callback )( )>
task_info resource_system::user_callback_task( pointer const in_this_ptr, u32 const in_thread_index )
{
	task_info info;
	info.functor = []( pvoid* const, u32 const, pointer const in_user_data )
	{
		( ( (ThisType*)in_user_data )->*Callback )( );
	};
	info.user_data = in_this_ptr;
	info.thread_index = in_thread_index;
	return info;
}

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED