#ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_H_INCLUDED

#include <types.h>
#include <system/system_event.h>
#include <system/time.h>
#include <utils/engine_threads.h>
#include "queried_resources.h"
#include "resource_system_types.h"

namespace resource_system {

void create( u32 const in_thread_count );

void stop( );
void destroy( );

void execute_tasks(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
);

custom_task_context create_custom_tasks(
	u32 const in_task_count,
	task_info const& in_callback
);

void finish_cusom_task(
	custom_task_context const in_context
);

void create_resources(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
);

template<typename ... Cooks>
void create_resources(
	task_info const& in_callback,
	Cooks* ... in_cooks
);

void destroy_resources(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback
);

template<typename ... Resources>
void destroy_resources(
	task_info const& in_callback,
	Resources* ... in_resources
);

bool process_task( u32 const in_thread_index );
bool process_helper_task( );
sys::system_event const& queue_event( u32 const in_thread_index );
sys::system_event const& helper_queue_event( );

task_info null_callback( );

template<void ( *Callback )( queried_resources&, pointer const )>
task_info user_callback_task( pointer const in_user_data, u32 const in_thread_index = engine_thread_count );

template<typename ThisType, void ( ThisType::*Callback )( queried_resources& )>
task_info user_callback_task( pointer const in_this_ptr, u32 const in_thread_index = engine_thread_count );

template<void ( *Callback )( pointer const )>
task_info user_callback_task( pointer const in_user_data, u32 const in_thread_index = engine_thread_count );

template<typename ThisType, void ( ThisType::*Callback )( )>
task_info user_callback_task( pointer const in_this_ptr, u32 const in_thread_index = engine_thread_count );

} // namespace resource_system

#include "api_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED