#ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

class resource_cook;

void create( u32 const in_thread_count );

void stop( );
void destroy( );

template<typename ResourceType>
void register_resource_type( );

void query_resources(
	resource_cook* const* const in_cooks,
	u32 const in_cook_count,
	user_query_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size
);

void busy_thread_job( u32 const in_thread_index, u64 const in_time_limit );
void free_thread_job( u32 const in_helper_thread_index );
void helper_thread_job( u32 const in_helper_thread_index );

} // namespace resource_system

#include "api_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_H_INCLUDED