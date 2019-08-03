#ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_H_INCLUDED

#include <types.h>
#include <system/time.h>
#include "resource_system_types.h"

namespace resource_system {

class resource_cook;

void create( u32 const in_thread_count );

void stop( );
void destroy( );

void create_resources(
	query_info const* const in_queries,
	u32 const in_query_count,
	user_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size
);

template<typename ... Cooks>
void create_resources(
	user_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size,
	Cooks* ... in_cooks
);

void busy_thread_job( u32 const in_thread_index, sys::time const in_time_limit );
void free_thread_job( u32 const in_helper_thread_index );
void helper_thread_job( u32 const in_helper_thread_index );

} // namespace resource_system

#include "api_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED